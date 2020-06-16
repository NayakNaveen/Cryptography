
# include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#define MX 8


#define MSGLEN 80
#define TO 'E'


/* Rotor wirings */
char rotor[5][26]={
	/* Input "ABCDEFGHIJKLMNOPQRSTUVWXYZ" */
	/* 1: */ "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
	/* 2: */ "AJDKSIRUXBLHWTMCQGZNPYFVOE",
	/* 3: */ "BDFHJLCPRTXVZNYEIWGAKMUSQO",
	/* 4: */ "ESOVPZJAYQUIRHXLNFTGKDCMWB",
	/* 5: */ "VZBRGITYUPSDNHLXAWMJQOFECK" };
char ref[26]="YRUHQSLDPXNGOKMIEBFZCWVJAT";

char notch[5]="QEVJZ";

/* Encryption parameters follow */

typedef struct P
{
  char order[3];/*={ 1, 2, 3 };*/
  char rings[3];/*={ 'A','A','A' };*/
  char pos[3];/*={ 'A','A','A' };*/
  char plug[10];/*="AMTE";*/
} Params;

/*take a char and return its encoded version according to the
  encryption params, update params, i.e. advance wheels
  this part uses Fauzan Mirza's code*/
char scramble(char c, Params *p)
{
  int i, j, flag = 0;

		c=toupper(c);
		if (!isalpha(c))
			return -1;

		/* Step up first rotor */
		p->pos[0]++;
		if (p->pos[0]>'Z')
			p->pos[0] -= 26;

		/* Check if second rotor reached notch last time */
		if (flag)
		{
			/* Step up both second and third rotors */
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			p->pos[2]++;
			if (p->pos[2]>'Z')
				p->pos[2] -= 26;
			flag=0;
		}

		/*  Step up second rotor if first rotor reached notch */
		if (p->pos[0]==notch[p->order[0]-1])
		{
			p->pos[1]++;
			if (p->pos[1]>'Z')
				p->pos[1] -= 26;
			/* Set flag if second rotor reached notch */
			if (p->pos[1]==notch[p->order[1]-1])
				flag=1;
		}

		/*  Swap pairs of letters on the plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}

		/*  Rotors (forward) */
		for (i=0; i<3; i++)
		{
			c += p->pos[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i]-'A';
			if (c<'A')
				c += 26;

			c=rotor[p->order[i]-1][c-'A'];

			c += p->rings[i]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i]-'A';
			if (c<'A')
				c += 26;
		}

		/*  Reflecting rotor */
		c=ref[c-'A'];

		/*  Rotors (reverse) */
		for (i=3; i; i--)
		{
			c += p->pos[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->rings[i-1]-'A';
			if (c<'A')
				c += 26;

			for (j=0; j<26; j++)
				if (rotor[p->order[i-1]-1][j]==c)
					break;
			c=j+'A';

			c += p->rings[i-1]-'A';
			if (c>'Z')
				c -= 26;

			c -= p->pos[i-1]-'A';
			if (c<'A')
				c += 26;
		}

		/*  Plugboard */
		for (i=0; p->plug[i]; i+=2)
		{
			if (c==p->plug[i])
				c=p->plug[i+1];
			else if (c==p->plug[i+1])
				c=p->plug[i];
		}

  return c;
}

/*take a string, return encoded string*/
char *enigma(char *in, Params *p)
{
  int j;
  char out[MSGLEN];
  for(j = 0; j < strlen(in); j++)
  out[j] = scramble(in[j], p);
  out[j] = '\0';
  return out;
}
 char in[MSGLEN], s[MSGLEN];
/*read in a string, and pass it through enigma*/
void cypher(Params p)
{
 ////
  int c, i = 0;

  while((c = getchar()) != '\n')
  {
    in[i] = toupper(c);
    i++;
  }
  in[i] = '\0';
  strcpy(s, enigma(in, &p));
  printf("\nGenerated Cipher text is :\n%s\n", s);
}

/*given a cipher text, and a crib, test all possible settings of wheel order a, b, c*/
void rotate(int a, int b, int c, char *cyph, char *crib, char *plug, int *ct)
{
  Params p;

  p.order[0] = a;
  p.order[1] = b;
  p.order[2] = c;
  p.rings[0] = p.rings[1] = p.rings[2] = 'A';
  strcpy(p.plug, plug);
  for(p.pos[0] = 'A'; p.pos[0] <= 'Z'; p.pos[0]++)
  {
    for(p.pos[1] = 'A'; p.pos[1] <= 'Z'; p.pos[1]++)
    {
      for(p.pos[2] = 'A'; p.pos[2] <= 'Z'; p.pos[2]++)
      {
/*        for(p.rings[0] = 'A'; p.rings[0] <= 'Z'; p.rings[0]++)
        {
          for(p.rings[1] = 'A'; p.rings[1] <= 'Z'; p.rings[1]++)
          {
            for(p.rings[2] = 'A'; p.rings[2] <= 'Z'; p.rings[2]++)
            {
*/	      Params cp = p;
	      int i = 0;

	      while(strlen(crib) > i)
	      {
		if(cyph[i] != scramble(crib[i], &cp))
		break;
		else
		i++;
	      }
	      if(strlen(crib) == i)
	      {
		char s[MSGLEN];

		(*ct)++;
	        printf("Wheels %d %d %d Start %c %c %c Rings %c %c %c plug value \"%s\"\n",
                        p.order[0], p.order[1], p.order[2],
                        p.pos[0], p.pos[1], p.pos[2],
                        p.rings[0], p.rings[1], p.rings[2], p.plug);
		cp = p;
		strcpy(s, enigma(cyph, &cp));
                printf("%s decoded -> %s\n", cyph, s);
              }
/*            }
          }
        }
*/      }
    }
  }
}

/*do the whole check including steckering of up to two pairs of letters*/
void test(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  char A, B, C, D;
  int i = 0, cs;
  char s[5];

  strcpy(s, "");
  printf("Checking wheels %d %d %d\n",  a, b, c);
  for(cs = 0; cs < 3; cs++)
  {
    if(cs > 0)
    {
      for(A = 'A'; A <= TO; A++)
      {
        for(B = A + 1; B <= TO; B++)
        {
	  s[0] = A;
	  s[1] = B;
	  s[2] = '\0';
	  if(cs > 1)
	  {
	    for(C = A + 1; C <= TO; C++)
	    {
	      if(C == B)
	      continue;
	      for(D = C + 1; D <= TO; D++)
              {
	        if(D == A || D == B)
		continue;
		i++;
		s[2] = C;
		s[3] = D;
		s[4] = '\0';
                rotate(a, b, c, cyph, crib, s, ct);
	      }
            }
          }
	  else
	  rotate(a, b, c, cyph, crib, s, ct);
        }
      }
    }
    else
    rotate(a, b, c, cyph, crib, s, ct);
  }
}


/*run on all permutations of wheels a, b, c*/
void permute(int a, int b, int c, char *cyph, char *crib, int *ct)
{
  test(a, b, c, cyph, crib, ct);
  test(a, c, b, cyph, crib, ct);
  test(b, a, c, cyph, crib, ct);
  test(b, c, a, cyph, crib, ct);
  test(c, a, b, cyph, crib, ct);
  test(c, b, a, cyph, crib, ct);
}

/*all triples of five possible wheels*/
void permuteAll(char *cyph, char *crib)
{
  int ct = 0;

  permute(1, 2, 3, cyph, crib, &ct);
  permute(1, 2, 4, cyph, crib, &ct);
  permute(1, 2, 5, cyph, crib, &ct);
  permute(1, 3, 4, cyph, crib, &ct);
  permute(1, 3, 5, cyph, crib, &ct);
  permute(1, 4, 5, cyph, crib, &ct);
  permute(2, 3, 4, cyph, crib, &ct);
  permute(2, 3, 5, cyph, crib, &ct);
  permute(2, 4, 5, cyph, crib, &ct);
  permute(3, 4, 5, cyph, crib, &ct);
  printf("\nFound %d solutions.\n", ct);
}

/*helper to read a character*/
char readCh()
{
  char c, ret;

  while((c = getchar()) != '\n')
  ret = c;
  return ret;
}

/*init the starting position*/
void initParams(Params *p)
{
  int i;
  char c;

  printf("d)default values or u)user values: ");
  c = readCh();

  if(c != 'u')
  {
    for(i = 0; i < 3; i++)
    {
      p->order[i] = i + 1;
      p->rings[i] = 'A';
      p->pos[i] = 'A';
    }
    strcpy(p->plug, "");
  }
  else
  {
      for(i = 0; i < 3; i++)
      {
        printf("Wheel %d: ", i + 1);
        p->order[i] = readCh() - 48;
      }
      for(i = 0; i < 3; i++)
      {
/*        printf("Ring  %d: ", i + 1);*/
        p->rings[i] = 'A';/*readCh();*/
      }
      for(i = 0; i < 3; i++)
      {
        printf("Start %d: ", i + 1);
        p->pos[i] = readCh();
      }
      printf("Stecker: ");
      i = 0;
      while((c = getchar()) != '\n')
      {
        p->plug[i] = c;
	i++;
      }
      p->plug[i] = '\0';
  }
  printf("Wheels %d %d %d Start %c %c %c Rings %c %c %c plug value \"%s\"\n",
         p->order[0], p->order[1], p->order[2],
         p->pos[0], p->pos[1], p->pos[2],
         p->rings[0], p->rings[1], p->rings[2], p->plug);
 printf("\nEnter the plain text to generate cipher text\n");
}


/********************************************MAIN*********************************************/
void main(int argc, char *argv[])
{
  Params p;

  if(argc > 2)  /*bombe case*/
  {
    permuteAll(argv[1], argv[2]);
  }
  else
  {
    initParams(&p);
    cypher(p);
  }

  int i, j, k = 0, l, m = 0, n;
    char key[MX][MX], keyminus[64], keystr[64], str[64] = {
        0
    };
    char alpa[64] = {
        'E','T','A','O','I','N','S','H','R','D','L','C','U','M','W','F','G','Y','P','B','V','K','J',
        'X','Q','Z','1','0','2','5','3','6','4','7','9','8','!','"','#','$','&','%','(',')','*','+',
        '-','.','/',':',';','<','=','>', '?','@','[','\\',']','^','_','`','{', '|', '}'
    };
    printf("\nEnter key for playfair matrix:\n");
    gets(keystr);

    printf("\nEntered plain text is :\n %s",s);
    printf("\nGenerated 8X8 Playfair Matrix is :\n");


    n = strlen(keystr);
    //convert the characters to uppertext
    for (i = 0; i < n; i++) {
        if (keystr[i] == 'j') keystr[i] = 'i';
        else if (keystr[i] == 'J') keystr[i] = 'I';
        keystr[i] = toupper(keystr[i]);
    }
    //convert all the characters of plaintext to uppertext
    for (i = 0; i < strlen(s); i++) {
        if (s[i] == 'j') s[i] = 'i';
        else if (s[i] == 'J') s[i] = 'I';
        s[i] = toupper(s[i]);
    }
    // store all characters except key
    j = 0;
    for (i = 0; i < 64; i++) {
        for (k = 0; k < n; k++) {
            if (keystr[k] == alpa[i]) break;
            else if (alpa[i] == 'J') break;
        }
        if (k == n) {
            keyminus[j] = alpa[i];
            j++;
        }
    }
    //construct key keymatrix
    k = 0;
    for (i = 0; i < MX; i++) {
        for (j = 0; j < MX; j++) {
            if (k < n) {
                key[i][j] = keystr[k];
                k++;
            } else {
                key[i][j] = keyminus[m];
                m++;
            }
            printf("%c ", key[i][j]);
        }
        printf("\n");
    }
    // construct diagram and convert to cipher text
    printf("\nGenerated Cipher Text is :");
    for (i = 0; i < strlen(s); i++) {
        if (s[i] == 'J') s[i] = 'I';
        if (s[i + 1] == '\0') playfair(s[i], 'X', key);
        else {
            if (s[i + 1] == 'J') s[i + 1] = 'I';
            if (s[i] == s[i + 1]) playfair(s[i], 'X', key);
            else {
                playfair(s[i], s[i + 1], key);
                i++;
            }
        }
    }
    getch();
}

void playfair(char ch1, char ch2, char key[MX][MX]) {
    int i, j, w, x, y, z;
    FILE * out;
    if ((out = fopen("cipher.txt", "a+")) == NULL) {
        printf("File Currupted.");
    }
    for (i = 0; i < MX; i++) {
        for (j = 0; j < MX; j++) {
            if (ch1 == key[i][j]) {
                w = i;
                x = j;
            } else if (ch2 == key[i][j]) {
                y = i;
                z = j;
            }
        }
    }
    //printf("%d%d %d%d",w,x,y,z);
    if (w == y) {
        x = (x + 1) % 8;
        z = (z + 1) % 8;
        printf("%c%c", key[w][x], key[y][z]);
        fprintf(out, "%c%c", key[w][x], key[y][z]);
    } else if (x == z) {
        w = (w + 1) % 8;
        y = (y + 1) % 8;
        printf("%c%c", key[w][x], key[y][z]);
        fprintf(out, "%c%c", key[w][x], key[y][z]);
    } else {
        printf("%c%c", key[w][z], key[y][x]);
        fprintf(out, "%c%c", key[w][z], key[y][x]);
    }
    fclose(out);
}
















