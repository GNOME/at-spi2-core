#!/bin/bash

if [ "$#" = 0 ]
then
	echo "Usage: $0 /path/to/keysymdef.h"
	exit 1
fi

# We are only interested in
# - keysyms which have well-defined unicode equivalent
# - and are not just trivial unicode keysyms
# - non-latin1 keysyms
# - not the lamda aliases
# and we tinker with the alias parentheses to make sorting easier

grep '^#define' "$1" | \
	grep -i "U+" | \
	grep -vi "0x100[0-9a-f][0-9a-f][0-9a-f][0-9a-f]" | \
	grep -vi " 0x00[0-9a-f][0-9a-f] " | \
	grep -vi "_lamda " | \
	sed -e 's/\/\*(/& /' | \
	sed -e 's/)\*\// &/' | \
	sort -k 5 | \
	perl -CS -e '
my $last = 0;
while (<>) {
	chomp;
	if ( /^\#define XK_([a-zA-Z_0-9]+)(\s*)   0x([0-9a-f]+)\s*\/\*(\(?) U\+([0-9A-F]{4,6}) (.*) \)?\*\/\s*$/ ) {
		my ( $xk, $space, $keysym, $paren, $unicode, $unistr ) = ( $1, $2, $3, $4, $5, $6);
		$unicode = hex("0x".$unicode);

		print "\n" if (int($unicode / 256) != int($last / 256));
		$last = $unicode;

		printf "  { 0x$keysym, 0x%04x }, /* $space$xk %lc $unistr */\n", $unicode, $unicode;
	}
}
	'
