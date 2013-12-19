import functools, operator, re, collections

fields = ['type', 'direction', 'endpoint', 'data']
ParserElement = collections.namedtuple('ParserElement', fields)

def parse(data):
	lines = data.replace("\r", "").split("\n")
	regex = '^(BULK|CTRL|INTR|ISOC)_(IN|OUT)\(([0-9.]+)\):\s*([0-9a-f]+)'
	matcher = functools.partial(re.search, regex)
	matches = map(matcher, lines)

	result = []
	for match_object in matches:
		if match_object is None: continue
		fields_dict = dict(zip(fields, match_object.groups()))
		result.append(ParserElement(**fields_dict))
	return result

def parse_file(filename):
	with open(filename, 'r') as content_file:
		return parse(content_file.read())
