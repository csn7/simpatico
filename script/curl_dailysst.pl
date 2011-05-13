#! /usr/bin/env perl

use strict;
use Date::Simple qw{:all};

my @MONTH = qw{Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec};
my $URL = "http://goos.kishou.go.jp/rrtdb/usr/pub/JMA/dailysst/%04d/dailysst.%s%02d";

my $username = shift;
my $password = shift;
my $year = shift;
my $month = shift;

my $i = ymd $year, $month, 1;
my $end = $i + 31;
$end = ymd $end->year, $end->month, 1;

for (; $i < $end; ++$i) {
  my $url = sprintf $URL, $i->year, $MONTH[$i->month - 1], $i->day;
  print qq{curl -u "$username:$password" -O "$url"\n};
}

=head1 NAME

curl_dailysst.pl

=head1 SYNOPSIS

curl_dailysst.pl USERNAME PASSWORD 2011 3 | sh -e

=cut
