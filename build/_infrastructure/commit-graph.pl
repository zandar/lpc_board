#!/usr/bin/perl -w

use Getopt::Std;

my ($opt_l);
&getopts("l");

open GNUPLOT, "|gnuplot -persist";
$data = "/tmp/commit-graph-$$.dat";
print "Using file $data\n";
open DATA, ">$data";
use FileHandle;
GNUPLOT->autoflush(1);
print GNUPLOT <<EOF;
set xdata time
set timefmt "%s"
set format x "%y/%m/%d" 
set xlabel "Date"
set grid
set key left top
EOF
%authors = ('Michal' => 'sojka',
	    'Martin' => '\(zidek\|mzi\|martin-eee\|martin-nb2\)',
	    'Filip'  => 'jares',
	    'Jarda'  => 'jarin',
	    'Jirka'  => '\(jirka\|kubia\)',
	    'Marek'  => '\(marek\|duch\)',
	    'Petr'   => 'benes',
	    "Celkem" =>	'.');

foreach $author(sort keys %authors) {
        $cmd = qq/git log --pretty=format:"commit %ct %s" --author='$authors{$author}' --no-merges --reverse --numstat @ARGV/;
	print "$cmd\n";
	@log = `$cmd`;
	$num_commits = 0;
	$added_lines = 0;
	foreach (@log) {
	    chomp;
	    if (/^commit (\d+)/) { $time=$1; $num_commits++; $printed=0; }
	    elsif (/(\d+)\s+(\d+)/) {
		$added_lines += $1;
	    } elsif (!$_) {
		print DATA "$time $num_commits $added_lines\n";
		$printed=1;
	    }
	}
	if ($num_commits > 0) {
	    if (!$printed) {
		print DATA "$time $num_commits $added_lines\n";
	    }
	    print DATA "\n\n";
	} else {
	    delete $authors{$author};
	}
}

if (!$opt_l) {
    @plots = ();
    $i=0;
    foreach $author(sort keys %authors) {
	push @plots, qq/"$data" index $i using 1:2 with linespoints title "$author"/;
	$i++;
    }
    print GNUPLOT "set ylabel \"Number of commits\"\n";
    print GNUPLOT "plot ", join(", ", @plots), ";\n";
} else {
    @plots = ();
    $i=0;
    foreach $author(sort keys %authors) {
	push @plots, qq/"$data" index $i using 1:3 with linespoints title "$author"/;
	$i++;
    }
    print GNUPLOT "set ylabel \"Added lines\"\n";
    # print GNUPLOT "set logscale y\n";
    print GNUPLOT "plot ", join(", ", @plots), ";\n";
}
