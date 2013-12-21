#include <stdio.h>

/*

  This small piece of code will take a ROM .are file
  and report all rooms that leads to other areas.

  Do whatever you want with it.

  . Maelduin

*/

FILE *areaf;
char buf[100];

int waitforline(char *pattern);
void parse_room();

int main(int argc, char *argv[])
{
    int vnuma, vnumb, vnum;
    
    if(argc==1 || argc>2)
    {
	printf("Usage: area [area file]\n");
	return 1;
    }

    if((areaf = fopen(argv[1], "r")) == NULL)
    {
	fprintf(stderr, "Error: Could not open %s for reading.\n", argv[1]);
	return 1;
    }
    
    fgets(buf, sizeof(buf), areaf);

    if(strncmp(buf, "#AREADATA", 8))
    {
	fprintf(stderr, "Error: %s is not an intact .are file? :(\n", argv[1]);
	return 1;
    }

    printf("Found area header.\n");

    waitforline("VNUMs");

    if((sscanf(buf, "VNUMs %d %d", &vnuma, &vnumb) != 2))
    {
	fprintf(stderr, "Error: Could not determine VNUM range.\n");
	return 1;
    }

    printf("Vnum range is %d -> %d.\n", vnuma, vnumb);
    waitforline("#ROOMS");
    parse_room(vnuma, vnumb);

    return 0; 
}

int  waitforline(char *pattern)
{
    int found = 0;

    while(found == 0)
    {
	fgets(buf, sizeof(buf), areaf);
	if(!strncmp(buf, pattern, sizeof(pattern)))
	{
	    found = 1;
	    printf("%s found.\n", pattern);
	}

	// This should never happen, you noob :D.

	if(feof(areaf))
	{
	    fprintf(stderr, "Error: %s not found .\n", pattern);
	    exit (1);
	}
    }
}

void parse_room(int vnuma, int vnumb)
{
    int vnum, dvnum, targetvnum, slask;
    int doors=0;

    fgets(buf, sizeof(buf), areaf);
    while((sscanf(buf, "#%d", &vnum) != 1))
    {
	fgets(buf, sizeof(buf), areaf);
    }
    
    if(vnum == 0)
    {
	printf("End of #ROOMS section.\n");
	exit(0);
    }

    for(doors=0; doors < 6; doors++)
    {
	while((sscanf(buf, "D%d", &dvnum) != 1))
	{
	    fgets(buf, sizeof(buf), areaf);

	    if(!strncmp(buf, "S\n", 2))
	    {
		parse_room(vnuma, vnumb);
	    }
	}
	
	while((sscanf(buf, "%d %d %d", &slask, &slask,  &targetvnum) != 3))
	{
	    if(sscanf(buf, "%s %d %d", &slask, &slask, &targetvnum) == 3)
	    {
		break;
	    }
	    fgets(buf, sizeof(buf), areaf);		
	}
	
	if(targetvnum > vnumb ||
	   targetvnum < vnuma)
	    printf("Area Link: [%d] has an exit to [%d]\n", vnum, targetvnum);
    }
}
