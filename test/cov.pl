#!/usr/bin/perl -w

my $funcw = 25;    # Width of function name column
my %funcs;         # All functions found

parse_gcov( "gcov -n main.cpp -f | c++filt" );
parse_gcov( "gcov -o ../armv7 -n function-dbg.gcno -f | c++filt" );
print_results();

# Print coverage statistics in columns and in alphabetical order
sub print_results
{
    my $cov_head  = "Coverage";
    my $name_head = "Function";

    # Print header
    printf("%10s ", $cov_head);
    printf("%-".$funcw."s \n", $name_head);

    printf("%10s ", "=" x length( $cov_head ) );
    printf("%-".$funcw."s \n", "=" x length( $name_head ));

    # Print coverage statistics
    for $func ( sort keys ( %funcs ) )
    {
        my $cov = $funcs{ $func };
        
        $func =~ /([\w]+)(<.*>)?/;
        my $name = $1;
        my $tmpl = $2;
        
        printf("%9s%% ", $cov);
        printf("%-".$funcw."s ", $name);
        print $tmpl if defined( $tmpl );
        print "\n";
    }
}

# Find functions that begin with arm::
sub parse_gcov
{
    my $cmd  = pop;
    my $func = undef;

    open GCOV, $cmd."|" or die ( "error running gcov" );
    while ( <GCOV> )
    {
        if ( defined( $func ) && /Lines executed:(\d{1,3}\.\d{1,2})/ )
        {
            $funcs{$func} = $1;
            $func = undef;
        }
        elsif ( /^Function \'.*?arm::([\w]+(<.*>)?)\(.*?\)/ )
        {
            $func = $1;
        }
    }
    close GCOV;
}

