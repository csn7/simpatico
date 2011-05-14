#! /usr/bin/env perl

use strict;
use Date::Simple qw{:all};

my @MONTH = qw{Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec};

my $URL = "http://database.rish.kyoto-u.ac.jp/arch/jmadata/data/gpv/original/%04d/%02d/%02d";
my $FILE0 = "Z__C_RJTD_%04d%02d%02d%02d0000_MSM_GPV_Rjp_L-pall_FH00-15_grib2.bin";
my $FILE1 = "Z__C_RJTD_%04d%02d%02d%02d0000_MSM_GPV_Rjp_Lsurf_FH00-15_grib2.bin";

my $year = shift;
my $month = shift;

my $i = ymd $year, $month, 1;
my $end = $i + 31;
$end = ymd $end->year, $end->month, 1;

for (; $i < $end; ++$i) {
  my $url = sprintf $URL, $i->year, $i->month, $i->day;
  for (my $j = 0; $j < 24; $j += 3) {
    my $file0 = sprintf $FILE0, $i->year, $i->month, $i->day, $j;
    my $file1 = sprintf $FILE0, $i->year, $i->month, $i->day, $j;
    print << "EOH";
curl --limit-rate 1M -O "$url/$file0"
curl --limit-rate 1M -O "$url/$file1"
EOH
  }
}

=head1 NAME

curl_msm.pl

=head1 SYNOPSIS

curl_msm.pl 2011 3 | sh -e

=cut
