import pygtk
pygtk.require('2.0')
import gtk, os, subprocess, operator, json, itertools, string
import regex2parser, hexeditor

LSUsbParser = regex2parser.create_parser('LSUsbResult', 'ID (\w+):(\w+) (.*)', ['vid', 'pid', 'name'])

FileParser = regex2parser.create_parser('FileParserResult',
	'^(BULK|CTRL|INTR|ISOC)_(IN|OUT)\(([0-9.]+)\):\s*([0-9a-f]+)',
	['type', 'direction', 'endpoint', 'data'])

class UsbSniff:
	settings = {}
	settings_dirname = "%s/.usbsniff" % os.environ['HOME']
	settings_path = "%s/settings.json" % settings_dirname
	menus = []

	def __init__(self):
		self.load_settings()

		# Main Menu
		menu_file = gtk.Menu()
		menu_file.append(gtk.MenuItem("_Open"))
		menu_file.append(gtk.MenuItem("_Save"))
		item_file = gtk.MenuItem("_File")
		item_file.set_submenu(menu_file)
		self.menus.append(menu_file)

		menu_capture = gtk.Menu()
		menu_capture.append(gtk.MenuItem("_Start"))
		menu_capture.append(gtk.MenuItem("_Clear"))
		menu_capture.append(gtk.MenuItem("_Reset device"))
		item_capture = gtk.MenuItem("_Capture")
		item_capture.set_submenu(menu_capture)
		self.menus.append(menu_capture)

		menu_view = gtk.Menu()
		menu_view.append(gtk.MenuItem("Toolbar"))
		menu_view.append(gtk.MenuItem("Hex"))
		item_view = gtk.MenuItem("_View")
		item_view.set_submenu(menu_view)
		self.menus.append(menu_view)

		menubar = gtk.MenuBar()
		menubar.append(item_file)
		menubar.append(item_capture)
		menubar.append(item_view)
		menubar.show()

		# Toolbar
		self.devices = []
		self.device_selector = gtk.combo_box_new_text()
		for dev in LSUsbParser.parse(subprocess.check_output("lsusb")):
			self.devices.append(dev)
			self.device_selector.append_text("%s:%s %s" % (dev.vid, dev.pid, dev.name))
		self.device_selector.set_active(0)
		self.device_selector.connect('changed', self.on_device_selector_changed)

		self.toolbar = gtk.Toolbar()
		self.toolbar_hbox = gtk.HBox()
		self.toolbar_hbox.pack_start(gtk.ToolButton(gtk.STOCK_MEDIA_RECORD), True, False, 0)
		self.toolbar_hbox.pack_start(gtk.ToolButton(gtk.STOCK_MEDIA_PLAY), True, False, 0)
		self.toolbar_hbox.pack_end(self.device_selector, True, False, 0)
		self.toolbar.add(self.toolbar_hbox)

		# Trying to preselect device by using last vid / pid that was saved in settings
		if 'vid' in self.settings:
			settings = self.settings
			candidates = filter(lambda t: (t.vid, t.pid) == (settings['vid'], settings['pid']), self.devices)
			pos = self.devices.index(candidates[0]) if len(candidates) else None
			if pos > 0: self.device_selector.set_active(pos)

		# TreeView
		self.liststore = gtk.ListStore(*[str] * 5)
		self.treeview = gtk.TreeView(self.liststore)
		self.treeview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
		self.treeview.connect('cursor-changed', self.on_cursor_changed)
		self.treeview.connect('button-press-event', self.on_treeview_button_press_event)
		for (index, column) in enumerate(['#', '', '', 'EP', '', '']):
			tvcolumn = gtk.TreeViewColumn(column)
			if index == 4:
				tvcolumn.set_expand(True)
				tvcolumn.set_max_width(300)
			cell = gtk.CellRendererText()
			tvcolumn.pack_start(cell, True)
			tvcolumn.set_attributes(cell, text=index)
			self.treeview.append_column(tvcolumn)
		self.treeview_scroller = gtk.ScrolledWindow()
		self.treeview_scroller.add(self.treeview)

		# TreeView popup menu
		self.treeview_popup_menu = gtk.Menu()
		self.treeview_popup_menu.append(gtk.MenuItem("Mark as selection"));
		self.treeview_popup_menu.append(gtk.MenuItem("Clear selection"));
		self.treeview_popup_menu.show_all()
		self.menus.append(self.treeview_popup_menu)

		# HexEditor
		self.hexeditor = hexeditor.HexEditor()
		self.hexeditor_scroller = gtk.ScrolledWindow()
		self.hexeditor_scroller.add(self.hexeditor)

		self.paned = gtk.VPaned()
		self.paned.pack1(self.treeview_scroller, resize=True)
		self.paned.pack2(self.hexeditor_scroller)
		self.paned.set_position(200)

		# Main Window & VBox
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.connect("destroy", self.destroy)
		self.window.set_size_request(800, 600)
		self.vbox = gtk.VBox()
		self.vbox.pack_start(menubar, False, False, 0)
		self.vbox.pack_start(self.toolbar, False, False, 0)
		self.vbox.pack_end(self.paned, True, True, 2)
		self.window.add(self.vbox)
		self.window.show_all()

		# Connecting router to all menu items
		get_children = operator.methodcaller('get_children')
		for child in reduce(operator.add, map (get_children, self.menus), []):
			child.connect("activate", self.on_menuitem_activated)

		self.load_file('/home/minoru/git/usbsniff/log')

	def main(self):
		gtk.main()
	
	def destroy(self, widget):
		self.save_settings()
		gtk.main_quit()
	
	def load_file(self, filename):
		content_file = open(filename, 'r')
		self.packets = []
		self.liststore.clear()
		for (index, row) in enumerate(filter(None, FileParser.parse(content_file.read()))):
			self.packets.append(row)
			self.liststore.append([index+1] + list(row))
	
	def load_settings(self):
		if not os.path.exists(self.settings_path):
			self.save_settings()
		with open(self.settings_path, 'r') as myFile:
			self.settings = json.loads(myFile.read())

	def save_settings(self):
		if not os.path.isdir(self.settings_dirname):
			os.mkdir(self.settings_dirname, 0777)
		with open(self.settings_path, 'w') as myFile:
			myFile.write(json.dumps(self.settings))
	
	def enumerate_packets(self, selected_packets=[]):
		for row in self.liststore:
			row[0] = ''
		counter = itertools.count(1)
		for i, packet in enumerate(self.packets):
			if (i, packet) in (selected_packets or [(i, packet)]):
				self.liststore[i][0] = counter.next()

	# Events
	
	def on_menuitem_activated(self, menuitem):
		# Route the request to a specific menu handler
		symbols = string.letters + ' '
		isalpha = lambda t: t in symbols
		normalized_label = filter(isalpha, menuitem.get_label()).lower()
		getattr(self, 'on_menu_%s' % normalized_label.replace(' ','_'))()
		return True

	def on_menu_open(self):
		dialog = gtk.FileChooserDialog("Open..",
			None,
			gtk.FILE_CHOOSER_ACTION_OPEN,
			(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
			gtk.STOCK_OPEN, gtk.RESPONSE_OK))
		if dialog.run() == gtk.RESPONSE_OK:
			self.load_file(dialog.get_filename())
		dialog.destroy()
	
	def on_menu_mark_as_selection(self):
		selection = self.treeview.get_selection().get_selected_rows()[1]
		self.selected_packets = [ (row[0], self.packets[row[0]]) for row in selection ]
		self.enumerate_packets(self.selected_packets)

	def on_menu_clear_selection(self):
		self.selected_packets = []
		self.enumerate_packets()
	
	def on_device_selector_changed(self, widget):
		device = self.devices[widget.get_active()]
		settings = self.settings
		(settings['vid'], settings['pid']) = (device.vid, device.pid)
	
	def on_cursor_changed(self, treeview):
		index = treeview.get_selection().get_selected_rows()[1][0][0]
		self.hexeditor.set_buffer(hex_to_array(self.packets[index].data))

	def on_treeview_button_press_event(self, widget, event):
		if event.button == 3:
			self.treeview_popup_menu.popup( None, None, None, event.button, event.time)
			return True

def hex_to_array(str):
	result = []
	counter = itertools.count(0, 2)
	for i in itertools.islice(counter, len(str)/2):
		result.append(int(str[i:i+2], 16))
	return(result)

if __name__ == "__main__":
	usbsniff = UsbSniff()
	usbsniff.main()
