import pygtk
pygtk.require('2.0')
import gtk, os
import operator

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
			child.connect("activate", self.menuitem_response)

		menubar = gtk.MenuBar()
		menubar.append(item_file)
		menubar.append(item_capture)
		menubar.append(item_view)
		menubar.show()

		# Toolbar
		self.toolbar = gtk.Toolbar()
		self.toolbar.append_item("test", "test", "test", None, None)

		# TreeView
		self.liststore = gtk.ListStore(int, *[str] * 4)
		self.treeview = gtk.TreeView(self.liststore)
		self.treeview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
		self.liststore.append([1, "BULK_OUT", "0.1", "1111", "Test write"])
		self.liststore.append([2, "BULK_IN", "0.1", "1111", "Test read"])
		for (index, column) in enumerate(['#'] + [''] * 4):
			tvcolumn = gtk.TreeViewColumn(column)
			if index == 3: tvcolumn.set_expand(True)
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
	
	def menuitem_response(self, menuitem):
		print menuitem

if __name__ == "__main__":
	usbsniff = UsbSniff()
	usbsniff.main()
