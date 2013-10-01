package USBSniff::Parser;
use strict;

=pod
A Perl binding for dealing with the Human-Readable Format.
Synopsis:
  my @packets = USBSniff::Parser::parse_file($ARGV[0]);
  for(@packets) {
  	$_->set_comment("Left button pressed") if $_->get_data =~ /^01/;
  }
  USBSniff::Parser::dump(@packets);
=cut

sub parse {
	my ($data) = @_;
	my @ret;
	my $line;
	for(split /\n/, $data) {
		$line++;
		my $cmt = $1 if s/#(.*)//;
		s/\s*$//;
		if($_ =~ /^(BULK|CTRL|INTR|ISOC)_(IN|OUT)\(([0-9.]+)\):\s*([0-9a-f]+)/) {
			die("Odd number of octets at $line") if length($4) % 2;
			my $element = USBSniff::Parser::Element->new();
			$element->set_comment($cmt);
			$element->set_type($1);
			$element->set_direction($2);
			$element->set_endpoint($3);
			$element->set_data($4);
			push @ret, $element;
		}
	}
	return(@ret);
}

sub parse_file {
	my ($fn) = @_;
	local $/ = '';
	open FILE, $fn || return undef;
	return parse(<FILE>);
	close FILE;
}

sub dump {
	$_->dump for @_;
}

package USBSniff::Parser::Element;
use strict;

sub new {
	return(bless({}, shift));
}

sub dump {
	print $_->as_string, "\n";
}

sub as_string {
	my ($self) = @_;
	my $ret = "$self->{type}_$self->{direction}($self->{endpoint}): $self->{data}";
	$ret .= " # $self->{comment}" if $self->{comment};
}

sub get_comment { return $_[0]->{comment}; }
sub set_comment { $_[0]->{comment} = $_[1]; }
sub get_type { return $_[0]->{type}; }
sub set_type { $_[0]->{type} = $_[1]; }
sub get_direction { return $_[0]->{direction}; }
sub set_direction { $_[0]->{direction} = $_[1]; }
sub get_endpoint { return $_[0]->{endpoint}; }
sub set_endpoint { $_[0]->{endpoint} = $_[1]; }
sub get_data { return $_[0]->{data}; }
sub set_data { $_[0]->{data} = $_[1]; }
sub get_binary_data { return pack("H*", $_[0]->{data}); }

1;
