/*
 *27jan2021
 * Found this program, titled "MIF_generator,c" in dge/bin
 * (on memory stick) dated 01nov2011 (from my time at Corning).
 * I don't remember what's an MIF, but it's an example of the
 * type of programming I need to perform for netlist translation.
 *31jan2021
 * Original program was, after boilplate striped from OrCAD file
 * and created for KiCAD file, was two while() loops with cryptic
 * test limits. Second loop to create node list for KiCAD file
 * contain two do-while loops. I made the logic work after much
 * trial and error, but it was difficult to understand. The
 * attempt here is cleaner--I think. The outermost while() loop
 * contains a switch statement with four cases -- the delimiters
 * "[", "]", "(", and ")".
 * The multiple connections within the node definitions is another
 * while() loop driven by "(" (to open), and ")" (to close) as
 * seen in the OrCAD input.
 * Extensive (and hopefully adequate) error checking was added.
 */
#include <stdio.h>
#define MAXTOKEN 120
int get();
int  getSomeTok(FILE *fp, char *tok, int lim);
int  gettoken(FILE *fp, char *tok, int lim);
int  getline();
char lower();
void indents(FILE *fp, int level);
int	 whitespace();
// void cleanUpExit(int flag);

// global variable this file. I'm too lazy to carry pointer
// through getline, gettoken, and getSomeTok.
int nline=0;

main(int argc, char** argv){
float recip;
float Xk, Xkp;
int i, c, nodeCnt;
int bracketlevel, parenlvl_KiCAD, parenlvl_OrCAD, number, fill;
int ncomps,nfinal_comp,errflag;
int nnodeKiCAD;
int *pnt;
char tokenString[MAXTOKEN];
FILE *outfp, *infp;

if (argc != 3) {
	fprintf(stderr,"usage: test2\n");
	fprintf(stderr,"\tTango_input_netlist  KiCAD_output_file \n");
	fprintf(stderr,"\t(both files generally have .net extensions)\n\n");
	errflag=2;
	goto cleanUpExit;
}
/* ----------------------------------------------------- */
// printf("before first fopen\n");
infp = fopen( argv[1], "r");
if(infp == NULL){
	fprintf(stderr,"failed to open file %s\n", argv[1]);
	errflag=3;
	goto cleanUpExit;
}

// printf("before second fopen\n");
outfp = fopen( argv[2], "w");
if(outfp == NULL){
	fprintf(stderr,"failed to open file %s\n", argv[2]);
	errflag=4;
	goto cleanUpExit;
}
/* ----------------------------------------------------- */
/* ------- header/boilerplate for KiCAD's pcbnew ------- */
parenlvl_KiCAD= 0;
fprintf(outfp, "(export (version D)\n");	// open (export
parenlvl_KiCAD++;

indents(outfp, parenlvl_KiCAD);				// manage indents
fprintf(outfp, "(design\n");			// open (design
parenlvl_KiCAD++;

indents(outfp, parenlvl_KiCAD);
fprintf(outfp, "(tool \"Eeschema (5.1.9)-1\")\n");
parenlvl_KiCAD--;

indents(outfp, parenlvl_KiCAD);
fprintf(outfp, ")\n");		// close design)
parenlvl_KiCAD--;

parenlvl_KiCAD++;
indents(outfp, parenlvl_KiCAD);
// printf("				parenlvl_KiCAD:%d\n", parenlvl_KiCAD);

fprintf(outfp, "(components\n");	// open (components, output file
parenlvl_KiCAD++;
// indents(outfp, parenlvl_KiCAD);
// printf("				parenlvl_KiCAD:%d\n", parenlvl_KiCAD);



// filecopy(infp);

*tokenString = '\0';
getline(infp, tokenString, MAXTOKEN);  // strip chaff OrCAD file
	printf("\ntoken returned:%s$\n", tokenString);
getline(infp, tokenString, MAXTOKEN);  // strip chaff OrCAD file
	printf("token returned:%s$\n", tokenString);

i=0;
ncomps = nfinal_comp = 0;
bracketlevel=0;
nnodeKiCAD=0;
parenlvl_OrCAD = 0;
// while (*tokenString != '[' && *tokenString != '(' ){	
while(1){
	getSomeTok(infp, tokenString, MAXTOKEN);
//	printf("token returned:%s$\n", tokenString);
//}
	switch (*tokenString){
		case '\0':
			/* getSomeTok() always returns a token.
			 * If not, EOF seen and it's time to clean up.
			 */
			fprintf(stderr, "Component count: %d   ", nfinal_comp);
			fprintf(stderr, "KiCAD node count: %d\n", nnodeKiCAD);
			errflag=0;
			goto cleanUpExit;
		break;
		case '[':			// open OrCAD component definition
			ncomps++;		// increment OrCAD component count, sanity check
			if (nfinal_comp){
				fprintf(stderr, "Component list revisited OrCAD input file.\n");
				exit();
			}

			bracketlevel++;
		//	printf("bracketlevel: %d  ", bracketlevel);	// error detection
		//	printf("starting components definitions, OrCAD file\n");

			// get OrCAD component name
			if (getSomeTok(infp, tokenString, MAXTOKEN) == 0){
				errflag=1;
				goto cleanUpExit;
			}
			printf("token name returned:%s\n", tokenString);
			switch (lower(tokenString[0])){
				case 'c':
				case 'd':
				case 'j':
				case 'l':
				case 'q':
				case 'm':
				case 'r':
				case 's':
				case 'u':
					break;
				default:
					fprintf(stderr, "warning: unrecognized component, not C,D,J,L,Q,M,R,S,U\n");
			}

			indents(outfp, parenlvl_KiCAD);
			// open (comp KiCAD and write component name
			fprintf(outfp, "(comp (ref %s)\n", tokenString);
			parenlvl_KiCAD++;
			indents(outfp, parenlvl_KiCAD);
	
			// to guard against user embedded whitespace, we use getLINE
		   	// get OrCAD footprint, order important here
			getline(infp, tokenString, MAXTOKEN);
			fprintf(outfp, "(footprint %s)\n", tokenString);
			indents(outfp, parenlvl_KiCAD);
			getline(infp, tokenString, MAXTOKEN);  // get value using getline
			fprintf(outfp, "(value %s)\n", tokenString);
			parenlvl_KiCAD--;
			indents(outfp, parenlvl_KiCAD);
			fprintf(outfp, ")\n");		// close KiCAD comp)
		break;

		case ']':			// close OrCAD component definition
			bracketlevel--;
		//	printf("bracketlevel: %d     ", bracketlevel);
			if ( bracketlevel ) {				// better be zero
				fprintf(stderr, "imbalanced square brackets--only one level in Tango netlist. About ");
				errflag=5;
				goto cleanUpExit;
			}
		break;

		case '(':					// open OrCAD node definition
			if( nnodeKiCAD == 0){	// start nets section KiCAD
					
			//	printf("paren level KiCAD before opening (nets: %d\n", parenlvl_KiCAD);
			/* this should be unecessary if indent maintenence is correct,
			 * but we don't know till it happens... 
			 */
				parenlvl_KiCAD =1;

				indents(outfp, parenlvl_KiCAD);

				// close KiCAD components section first then open nets
				fprintf(outfp, "%s", ")\n");

				indents(outfp, parenlvl_KiCAD);
				fprintf(outfp, "%s", "(nets\n");	// open (nets KiCAD
				parenlvl_KiCAD++;
				indents(outfp, parenlvl_KiCAD);
				nnodeKiCAD++;
				nfinal_comp = ncomps;
				printf("OrCAD component count: %d\n\n", ncomps);
			}
			else
				nnodeKiCAD++;
			parenlvl_OrCAD++;						// error checking OrCAD
			// get OrCAD node name
			if (getSomeTok(infp, tokenString, MAXTOKEN) == 0){
				errflag=1;
				goto cleanUpExit;
			}
			// check for errant delimiters, we expect component name
			if ( tokenString[0] == '(' ||  tokenString[0] == ')' ||
			     tokenString[0] == '[' ||  tokenString[0] == ']' ){
				fprintf(stderr, "Errant delimiter OrCAD input. About ");
				errflag=5;
				goto cleanUpExit;
			}
			// write KiCAD node count and node name
			fprintf(outfp, "(net (code %d) (name %s)\n", nnodeKiCAD, tokenString);
		//	indents(outfp, parenlvl_KiCAD);
			parenlvl_KiCAD++;
			while (1) {
				// get OrCAD ref name on this node
				if (getSomeTok(infp, tokenString, MAXTOKEN) == 0){
					errflag=1;
					goto cleanUpExit;
				}
				if(*tokenString == ')'){ // we're done with this node)
					parenlvl_KiCAD--;
					indents(outfp, parenlvl_KiCAD);
					fprintf(outfp, ")\n");		// close node
					parenlvl_OrCAD--;

				//	printf("paren level KiCAD: %d  parenlvl OrCAD: %d\n",
				//	parenlvl_KiCAD, parenlvl_OrCAD);

					if(parenlvl_OrCAD){
						fprintf(stderr, "imbalanced parentheses OrCAD input. About ");
						errflag=5;
						goto cleanUpExit;
					}
					indents(outfp, parenlvl_KiCAD); // set indent for following node
					break;		// break while() loop building node connections
				}
				else if(*tokenString == '(' || *tokenString == '[' || *tokenString == ']'){
					fprintf(stderr, "imbalanced parentheses OrCAD input. About ");
					errflag=5;
					goto cleanUpExit;
				}
				else{
					indents(outfp, parenlvl_KiCAD); // more components this node
				}

				printf("token ref returned: %s$\n", tokenString);
				fprintf(outfp, "(node (ref %s) ", tokenString);	// KiCAD output
				// get pin number OrCAD
				if (( c = getSomeTok(infp, tokenString, MAXTOKEN)) == 0){
					errflag=1;
					goto cleanUpExit;
				}
				// ------------------------------------------------------------
				while ( --c >= 0){
					if (tokenString[c] >= '0' && tokenString[c] <= '9' ||
					    tokenString[c] >= 'a' && tokenString[c] <= 'z' ||
					    tokenString[c] >= 'A' && tokenString[c] <= 'Z');
					else{
						fprintf(stderr,"expecting alpha-numeric (some BGAs) in pin field OrCAD. Wonky character: %c. About ", tokenString[c]);
						errflag=5;
						goto cleanUpExit;
					}
				}
				// ------------------------------------------------------------
				fprintf(outfp, "(pin %s))\n", tokenString);		// KiCAD output
			}			// close while() loop generating node connections
		break;

		case ')':		// close OrCAD node definition
			fprintf(stderr, "unmatched parentheses closing OrCAD input. About ");
			errflag=5;
			goto cleanUpExit;
		break;
		default:
			fprintf(stderr, "error: unrecognized delimitor case OrCAD input ");
			errflag=5;
			goto cleanUpExit;
		break;
	}
  }

cleanUpExit:;

fprintf(stderr, "OrCAD line: %d\n\n", nline);
	if(errflag == 1)
		fprintf(stderr, "Unexpected EOF, seen OrCAD input   ");
	if(errflag == 2)	// usage hint, top of file.
		exit();
	if(errflag == 3)	// failed to open OrCAD input
		exit();
	if(errflag == 4){	// failed ot open KiCAD output
		fclose(infp);
		exit();
	}

	while ( parenlvl_KiCAD){
		printf("close paren levels: %d\n", parenlvl_KiCAD);
		parenlvl_KiCAD--;
		fprintf(outfp, ")\n");		// close nets and export
		indents(outfp, parenlvl_KiCAD);
	}

	fclose(infp);
	fclose(outfp);
	exit();
}		/* close main() */
/* -------------------------------------------------------------------------- */
// from K&R, a classic WRONG! <char c> should be an int to properly handle EOF
filecopy(FILE *fp){
	char c;
	while ((c = getc(fp)) != EOF)
		putc(c, stdout);
}
/* ------------------------------------------------------ */
/* -- Only return if EOF or non-white characters found -- */
int  getSomeTok(FILE *fp, char *tok, int lim){
	int count;
	// returns token's lenght
	while ((count=gettoken(fp,tok,lim)) == 0)
		;							// try again
	return (count);
}
/* ------------------------------------------------------ */
int  gettoken(FILE *fp, char *tok, int lim){
	int c, nchar;
	nchar=0;
	while ((c = getc(fp)) != EOF && --lim > 0){
		if ( !whitespace(c) ){
			*tok++ = c;		// tack on end
			nchar++;
		}
		else{
			*tok = '\0';	// terminate string
			return(nchar);
		}
	}
	*tok = '\0';	// empty string
	return(c);		// probable EOF, hopefully non-zero
}
/* ------------------------------------------------------ */
int  getline(FILE *fp, char *tok, int lim){
	int c, nchar;
	nchar=0;
	while ((c = getc(fp)) != EOF && --lim > 0){
		if ( c != '\n' ){
			*tok++ = c;	// tack on end
			nchar++;
		}
		else{
			*tok = '\0';	// ignore newline and terminate string
			nline++;		// okay, so it's a global variable...
			return(nchar);
		}
	}
	*tok = '\0';	// zero string length
	return(c);		// probable EOF, hopefully non-zero
}
/* ------------------------------------------------------ */

int whitespace(char c){
	switch (c){
		case ' ':
		case '\t':
		case ',':
		case '-':
			return 1;
		case '\n':
			nline++;		// okay, so it's a global variable...
			return 1;
		default:
			return 0;
	}
}
// ---------------------------
char lower(char c){
	if (c >= 'A' && c <= 'Z')
		return (c + 'a' -'A');
	else
		return c;
}
/* ------------------------------------------------------ */
void indents(FILE *fp, int level){
	int j;
	for(j=0; j<level; j++)
			fprintf(fp, "  ");	// manage indents
}
/* ------------------------------------------------------ */
/* -------------------------------------------------------------------------- */
			//	printf("\ntoken number length: %d\n", c);
			//	printf("OrCAD file line number: %d\n", nline);
			//	printf("OrCAD file token returned: %s\n", tokenString);
				//		printf("true test branch, c: %d\n", c) ;	// proper digit
