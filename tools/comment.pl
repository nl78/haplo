#!/usr/bin/perl

my $comment=0;
my $done;

open(IN, "< comment.tmpl");
while(<IN>) {
	print;
}
close(IN);

while(<>) {
	if (($done == 0) && (m|/\*|)) {
		#s|/\*.*$||;
		#print;
		$comment=1;
	}

	if ($comment==0) {
		print;
	}
	
	if (($comment == 1) && (m|\*/|)) {
		#s|^.*\*/||;
		#print;
		$comment=0;
		$done=1;
	}
}
