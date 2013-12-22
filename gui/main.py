import pygtk
pygtk.require('2.0')
import gtk, os, subprocess
import operator
import regex2parser

LSUsbParser = regex2parser.create_parser('LSUsbResult', 'ID (\w+):(\w+) (.*)', ['vid', 'pid', 'name'])

FileParser = regex2parser.create_parser('FileParserResult',
	'^(BULK|CTRL|INTR|ISOC)_(IN|OUT)\(([0-9.]+)\):\s*([0-9a-f]+)',
	['type', 'direction', 'endpoint', 'data'])

class UsbSniff:
	def __init__(self):
		# Main Menu
		menu_file = gtk.Menu()
		menu_file.append(gtk.MenuItem("_Open"))
		menu_file.append(gtk.MenuItem("_Save"))
		item_file = gtk.MenuItem("_File")
		item_file.set_submenu(menu_file)

		menu_capture = gtk.Menu()
		menu_capture.append(gtk.MenuItem("_Start"))
		menu_capture.append(gtk.MenuItem("_Clear"))
		menu_capture.append(gtk.MenuItem("_Reset device"))
		item_capture = gtk.MenuItem("_Capture")
		item_capture.set_submenu(menu_capture)

		menu_view = gtk.Menu()
		menu_view.append(gtk.MenuItem("Toolbar"))
		menu_view.append(gtk.MenuItem("Hex"))
		item_view = gtk.MenuItem("_View")
		item_view.set_submenu(menu_view)

		get_children = operator.methodcaller('get_children')
		for child in reduce(operator.add, map (get_children, [menu_file, menu_capture, menu_view]), []):
			child.connect("activate", self.on_menuitem_activated)

		menubar = gtk.MenuBar()
		menubar.append(item_file)
		menubar.append(item_capture)
		menubar.append(item_view)
		menubar.show()

		# Toolbar
		self.device_selector = gtk.combo_box_new_text()
		for dev in LSUsbParser.parse(subprocess.check_output("lsusb")):
			self.device_selector.append_text("%s:%s %s" % (dev.vid, dev.pid, dev.name))
		self.device_selector.set_active(0)
		self.toolbar = gtk.Toolbar()
		self.toolbar_hbox = gtk.HBox()
		self.toolbar_hbox.pack_start(gtk.ToolButton(gtk.STOCK_MEDIA_RECORD), True, False, 0)
		self.toolbar_hbox.pack_start(gtk.ToolButton(gtk.STOCK_MEDIA_PLAY), True, False, 0)
		self.toolbar_hbox.pack_end(self.device_selector, True, False, 0)
		self.toolbar.add(self.toolbar_hbox)

		# TreeView
		self.liststore = gtk.ListStore(int, *[str] * 4)
		self.treeview = gtk.TreeView(self.liststore)
		self.treeview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
		for (index, column) in enumerate(['#', '', '', 'EP', '', '']):
			tvcolumn = gtk.TreeViewColumn(column)
			if index == 4: tvcolumn.set_expand(True)
			cell = gtk.CellRendererText()
			tvcolumn.pack_start(cell, True)
			tvcolumn.set_attributes(cell, text=index)
			self.treeview.append_column(tvcolumn)

		# Main Window & VBox
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.connect("destroy", gtk.main_quit)
		self.window.set_size_request(800, 600)
		self.vbox = gtk.VBox()
		self.vbox.pack_start(menubar, False, False, 0)
		self.vbox.pack_start(self.toolbar, False, False, 0)
		self.vbox.pack_end(self.treeview, True, True, 2)
		self.window.add(self.vbox)
		self.window.show_all()

	def main(self):
		gtk.main()
	
	def load_file(self, filename):
		content_file = open(filename, 'r')
		self.liststore.clear()
		for (index, row) in enumerate(filter(None, FileParser.parse(content_file.read()))):
			self.liststore.append([index+1] + list(row))

	# Events
	
	def on_menuitem_activated(self, menuitem):
		# Route the request to a specific menu handler
		isalpha = operator.methodcaller('isalpha')
		normalized_label = filter(isalpha, menuitem.get_label()).lower()
		getattr(self, 'on_menu_%s' % normalized_label)()

	def on_menu_open(self):
		dialog = gtk.FileChooserDialog("Open..",
			None,
			gtk.FILE_CHOOSER_ACTION_OPEN,
			(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
			gtk.STOCK_OPEN, gtk.RESPONSE_OK))
		if dialog.run() == gtk.RESPONSE_OK:
			self.load_file(dialog.get_filename())
		dialog.destroy()

if __name__ == "__main__":
	usbsniff = UsbSniff()
	usbsniff.main()
