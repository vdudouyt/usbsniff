import functools, re, collections

def match_against_field_names(regex, field_names, string):
	matched = re.search(regex, string)
	if matched is not None:
		return dict(zip(field_names, matched.groups()))

def create_parser(typename, regex, field_names):
	matcher = functools.partial(match_against_field_names, regex, field_names)

	NamedTupleClass = collections.namedtuple(typename, field_names)
	NamedTupleClass.from_string = staticmethod(lambda s:
			NamedTupleClass(**matcher(s)) if matcher(s) else None
		)
	NamedTupleClass.parse = staticmethod(lambda data:
			map(NamedTupleClass.from_string, data.strip().split("\n"))
		)
	return NamedTupleClass
