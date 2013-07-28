#!/usr/bin/perl -w

my $SINGLETON_NAME = "MenuMaker";
my $ID_PREFIX = "MENUMAKER_ID_";

my $PTR_SIZE = 4;
my $BYTE_PER_ENTRY = 3 + 2*$PTR_SIZE;
my $BYTE_PER_MENU = 2 + $PTR_SIZE + 32;

# 1. read all menu entries
# 2. collect all ids
# 3. create const ints from ids
# 4. associate menu entries and sub menues

use strict;
use JSON;
use Data::Dumper;

my $json_text = "";
open(FILE, "menumaker.json") or die $!;
while(<FILE>){$json_text.=$_;}
close(FILE);

open(FILE, ">MenuMaker_menu.h") or die $!;
my $all = decode_json($json_text);

my %ids;
my $stringId = 0;
my %string2id;
my %id2string;

sub assertString($) {
	my ($string) = @_;
	if(defined($string2id{$string})) {
		return $string2id{$string};
	}
	$string2id{$string} = $stringId;
	$id2string{$stringId} = $string;
	
	return $stringId++;
}

foreach my $menu_id (keys %{$all->{menu}}) {
	$ids{$menu_id} = 1 unless defined($ids{$menu_id});
	
	my $menu = $all->{menu}->{$menu_id};
	my $title = $menu->{title};
	$menu->{title_string_id} = assertString($title);

	foreach my $entry_id (keys %{$menu->{entries}}) {
		$ids{$entry_id} = 1 unless defined($ids{$entry_id});

		my $entry = $menu->{entries}->{$entry_id};
		$title = $entry->{title};
		$entry->{title_string_id} = assertString($title);
	}
}

printf FILE "
#ifndef MENUMAKER_MENU_H
#define MENUMAKER_MENU_H

#include \"MenuMaker.h\"

";

if(1==0) {
printf FILE "#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#else
 #define PROGMEM
#endif
";
}

my $textBytes = 0;
my $menuCount = 0;
my $entryCount = 0;

my $id_num = 0;
foreach my $id (keys %ids) {
	printf FILE "#define %s%s %d\n", 
		$ID_PREFIX, $id, $id_num++;
}
for(my $i=0;$i<$stringId;$i++) {
	printf FILE "const char *%sSTRING_%d = \"%s\";\n", 
		$ID_PREFIX, $i, $id2string{$i};
	
	$textBytes += (length($id2string{$i}) + 1);
}

printf FILE "void %s::onCreate() {\n",
	$SINGLETON_NAME;

foreach my $menu_id (keys %{$all->{menu}}) {
	my $menu = $all->{menu}->{$menu_id};
	my $select = 0;
	if (defined($menu->{select})){
		if ("single" eq $menu->{select}) {
			$select = 1;
		} elsif ("multi" eq $menu->{select}) {
			$select = 2;
		}
	}
	printf FILE "\tcreateMenu(%s%s, %sSTRING_%d, %d)\n", 
		$ID_PREFIX, $menu_id, $ID_PREFIX, $menu->{title_string_id}, $select;

	my %entries;
	foreach my $entry_id (keys %{$menu->{entries}}) {
		my $entry = $menu->{entries}->{$entry_id};
		
		my $order = $entry->{order};

		$entries{$order} = sprintf "->addEntry(%s%s, %sSTRING_%d,%s)",
			$ID_PREFIX, $entry_id, $ID_PREFIX, $entry->{title_string_id},
			defined($all->{menu}->{$entry_id})?"true":"false";
			
		$entryCount++;
	}
	my @a;
	foreach my $o (sort keys %entries) {
		push @a, $entries{$o};
	}
	print FILE "\t\t".join("\n\t\t", @a);
	printf FILE ";\n\n";
	
	$menuCount++;
}
printf FILE "}

#endif // MENUMAKER_MENU_H
";
close(FILE);

printf "constant RAM usage:\n\tStrings:\t%d\n\tMenus:\t\t%d\n\tEntries:\t%d\n\n\tTotal:\t\t%d\n",
	$textBytes, ($menuCount * $BYTE_PER_MENU), ($entryCount * $BYTE_PER_ENTRY),
	$textBytes + ($menuCount * $BYTE_PER_MENU) + ($entryCount * $BYTE_PER_ENTRY);
#printf "Using [%d] bytes for Menu strings\n", $textBytes;