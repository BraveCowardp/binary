#include <stdlib.h>
#include <stdio.h>
#include "balance.h"

extern struct field field[XMAX][YMAX];

static void make_line(int i, int dir, struct field *line)
{
    int j;
    int max;
    if (hori(dir))
	max = XMAX;
    else
	max = YMAX;
    
    for (j=0; j<max; j++) {
	init_link(&(line[i]));
	if (hori(dir)) {
	    line[j].value = field[j][i].value;
	    line[j].link.x = field[j][i].link.x;
	} else {
	    line[j].value = field[i][j].value;
	    line[j].link.x = field[i][j].link.y;
	}
    }
}

static void restore_line(int i, int dir, struct field *line)
{
    int j;
    int max;
    if (hori(dir))
	max = XMAX;
    else
	max = YMAX;
    
    for (j=0; j<max; j++) {
	if (hori(dir)) {
	    field[j][i].value = line[j].value;
	    field[j][i].link.x = line[j].link.x;
	    field[j][i].link.y = i;
	} else {
	    field[i][j].value = line[j].value;
	    field[i][j].link.y = line[j].link.y;
	    field[i][j].link.x = i;
	}
    }
}

void fill_remain(int max, struct field *line, struct count count)
{
    if ((count.zero == max/2) && (count.empty > -1))
	set_value(&(line[count.empty]), 1);
    if ((count.one == max/2) && (count.empty > -1))
	set_value(&(line[count.empty]), 0);
}

void link_remain(int max, struct field *f)
{
    int i;
    int temp = -1;
    for (i=0; i<max; i++) {
	if ((temp == -1) && (f[i].value == -1))
	    temp = i;
	if ((temp != -1) && (f[i].value == -1)) {
	    f[i].link.x = temp;
	    f[temp].link.x = i;
	}
    }
}

static void check_twin_link(struct field *f, int a, int b, int c, int d)
{
    if ((f[a].value != -1)
	&& (f[b].value == -1)
	&& (f[c].value == -1)
	&& (f[d].value == !(f[a].value))) {
	f[b].link.x = c;
	f[c].link.x = b;
    }
}

void twin_link(int max, struct field *f)
{
    int i;
    for (i=0; i<max; i++) {
	check_twin_link(f, i, i+1, i+2, i+3);
	check_twin_link(f, i, i-1, i-2, i-3);
    }
}

static void make_link(int max, struct field *f, struct count count)
{
    if (count.num == max-2)
	link_remain(max, f);
    twin_link(max, f);
}

static void complete_link(int i, struct field *f)
{
    int x;
    x = f[i].link.x;
    if (f[x].value != -1) {
	set_value(&(f[i]), !(f[x].value));
    }
}

void check_link(int max, struct field *line)
{
    int i;

    for (i=0; i<max; i++) {
	if (line[i].link.x != -1) {
	    complete_link(i, line);
	}
    }
}

static int make_count(short count_link, struct field *line, struct count *count)
{
    int counter = count_link;
    switch (line->value) {
    case 0:
	count->zero++;
	count->num++;
	break;
    case 1:
	count->one++;
	count->num++;
	break;
    case -1:
	if ((counter != -1) && (line->link.x != -1)) {
	    if (counter == 1) {
		count->one++;
		count->zero++;
		count->num += 2;
		counter = 0;
	    } else {
		counter++;
	    }
	}
	break;
    }
    return counter;
}

static void countline(int dir, struct field *line)
{
    int i;
    int max;
    int counter = -1;
    struct count count;

    if (hori(dir))
	max = XMAX;
    else
	max = YMAX;
    count.zero = count.one = count.num = 0;
    count.empty = -1;
    
    for (i=0; i<max; i++) {
	init_link(&(line[i]));
	make_count(counter, &(line[i]), &count);
	if (line[i].value == -1)
	    count.empty = i;
    }
    fill_remain(max, line, count);
    make_link(max, line, count);
    check_link(max, line);
    counter = 0;
    count.zero = count.one = count.num = 0;
    count.empty = -1;

    for (i=0; i<max; i++) {
	counter = make_count(counter, &(line[i]), &count);
	if ((line[i].value == -1) && (line[i].link.x == -1))
	    count.empty = i;
    }
    fill_remain(max, line, count);
}

void check_balance(int dir, int max)
{
    int i;
    struct field line[max];
    
    for (i=0; i<max; i++) {
	make_line(i, dir, line);
	countline(dir, line);
	restore_line(i, dir, line);
    }
}
