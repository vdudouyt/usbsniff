import pygtk, gtk, pango
import operator, string, functools, itertools, string

class HexEditor(gtk.TextView):

	_gtype_name__= "TableEdit"
	last_request = None
	digits = string.digits + string.ascii_letters[0:6]
	cols = rows = 0

	def __init__(self, buf = None):
		gtk.TextView.__init__(self)

		self.buf = buf
		self.modify_font(pango.FontDescription("monospace 12"))
		self.set_property('overwrite', True)
		self.set_property('wrap-mode', gtk.WRAP_NONE)
		self.connect('size-allocate', self.on_size_allocate)
		self.connect('key-press-event', self.on_insert_at_cursor)
	
	def set_buffer(self, buf):
		self.buf = buf
		self.update_text()
	
	def update_text(self):
		bytes_per_row = self.cols / 3
		if not bytes_per_row: return False
		formatter = functools.partial(operator.mod, "%02x ")

		counter = itertools.count(0)
		rows = []
		if self.buf is None:
			return
		for i, g in itertools.groupby(self.buf, lambda t: counter.next() / bytes_per_row):
			str_chunks = map(formatter, list(g))
			rows.append(reduce(operator.add, str_chunks))
		self.get_buffer().set_text(string.join(rows, "\n"))
	
	# Events
	def on_size_allocate(self, widget, rect):
		if self.last_request == rect:
			return True
		self.last_request = rect

		x, y, w, h = rect
		font_width, font_height = self.get_font_metrics()
		self.cols = w / font_width
		self.rows = h / font_height
		self.update_text()
	
	def on_insert_at_cursor(self, widget, event):
		keyname = gtk.gdk.keyval_name(event.keyval)
		if keyname in self.digits: return False
		if keyname in ['Up', 'Down', 'Left', 'Right', 'Home', 'End']: return False
#		where = self.get_buffer().get_iter_at_line_offset(5, 5)
#		self.get_buffer().place_cursor(where)
		return True

	# Utilities 
	def get_font_metrics(self):
		context = self.get_pango_context()
		layout = pango.Layout(context)
		layout.set_text(' ') # Assuming monospace
		return layout.get_pixel_size()
