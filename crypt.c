#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define SIZE_OF_MSG		(255)
#define AVERAGE_EXPAND	(10)
// Space separated
#define SEPERATOR_CHAR	'_'
#define SHIFT_SEPERATOR	","
#define UNKNOWN_SEQ_CHAR	'@'
#define ENTER_CHAR	0x0A

char *conversionTable[] = {
		"0.01", // a
		"0.02", // b
		"0.03", // c
		"0.04", // d
		"0.05", // e
		"0.06", // f
		"0.07", // g
		"0.001", // h
		"0.002", // i
		"0.003", // j
		"0.004", // k
		"0.005", // l
		"0.006", // m
		"0.007", // n
		"0.0001", // o
		"0.0002", // p
		"0.0003", // q
		"0.0004", // r
		"0.0005", // s
		"0.0006", // t
		"0.0007", // u
		"0.00001", // v
		"0.00002", // w
		"0.00003", // x
		"0.00004", // y
		"0.00005", // z
		
		
};
#define  CONV_TBL_SIZE	(sizeof(conversionTable)/sizeof(conversionTable[0]))

// TODO: FIXME: Only lower case for now...,
// TODO: FIXME: back space (erase character) not supported for now.
char *CryptEncode(const char *msg, int shift)
{
	static char ReversedMsg[SIZE_OF_MSG];
	static char EncodedMsg[SIZE_OF_MSG * AVERAGE_EXPAND];
	char *pcurrentchar;
	char ch;
	char shiftnum[10];
	char *pdestchar;
	
	memset(ReversedMsg,0,sizeof(ReversedMsg));
	memset(EncodedMsg,0,sizeof(EncodedMsg));
	pdestchar = &(ReversedMsg[0]);
	
	shift %= 26; // limit shift in 26 number..,
	if(NULL != msg)
	{
			pcurrentchar = (char *)msg;
			// Apply shift
			while(NULL != pcurrentchar && '\0' != *pcurrentchar)
			{
				ch = *pcurrentchar++;
				if(ENTER_CHAR == ch)
				{
					continue;
				}
				if(ch >= 'a' && ch <= 'z')
				{
				if((ch + shift) >= 'a' && (ch + shift) <= 'z')
				{
					ch += shift;

				}else if ((ch + shift) < 'a')
				{
					// Roll under
					ch = 'z' - ('a' - (ch + shift)) + 1;
				}else if ((ch + shift) > 'z')
				{
					// Roll over
					ch = 'a' + shift - 1;
				}
				else
				{
					printf("Unreachable statement\n");
					ch = ch;
				}
				} else
				{
					// Un supported char.
					ch = ch;
				}
				*pdestchar++ = ch;
			}
			
			// Lets encode:
			pcurrentchar = ReversedMsg;
			printf("Shifted string : %s\n",ReversedMsg);
			pdestchar    = EncodedMsg;
			while(NULL != pcurrentchar && '\0' != *pcurrentchar)
			{
				if(*pcurrentchar >= 'a' && *pcurrentchar <= 'z')
				{
					strcat(pdestchar,conversionTable[ *pcurrentchar - 'a']);
					pdestchar += strlen(conversionTable[*pcurrentchar - 'a']);
				} else
				{
					// If not in the table, use as it is.,
					*pdestchar++ = *pcurrentchar;
				}
				*pdestchar++ = SEPERATOR_CHAR;
				pcurrentchar++;
			}
			memset(shiftnum,0,sizeof(shiftnum));
			snprintf(shiftnum,sizeof(shiftnum),"%d", shift);
			strcat(EncodedMsg,SHIFT_SEPERATOR);
			strcat(EncodedMsg,shiftnum);
	}
	
	return EncodedMsg;
}

int toString(char* a) {
  int c, sign, offset, n;

  if (a[0] == '-') {  // Handle negative integers
    sign = -1;
  }

  if (sign == -1) {  // Set starting position to convert
    offset = 1;
  }
  else {
    offset = 0;
  }

  n = 0;

  for (c = offset; a[c] != '\0'; c++) {
    n = n * 10 + a[c] - '0';
  }

  if (sign == -1) {
    n = -n;
  }

  return n;
}

char ReverseLookup(char *code)
{
	int index;
	for(index = 0; index < CONV_TBL_SIZE; index++)
	{
		if(0 == strcmp(code,conversionTable[index]))
		{
			// found return char.
			return 'a' + index;
		}
	}
	return *code; // return unsupported char as is UNKNOWN_SEQ_CHAR;
}

// TODO: FIXME: Only lower case for now...,
char *CryptDecode(char *msg)
{
	static char RotatedMsg[SIZE_OF_MSG];
	static char DecodedMsg[SIZE_OF_MSG];
	char *pcurrentchar;
	char *pcurrentend;
	char *pshift;
	int   shift;
	int   index,i;
	char ch;
	char shiftnum[10];
	
	pshift = strrchr(msg,SEPERATOR_CHAR);
	if(NULL == pshift)
	{
		printf("Could not find shift in the messge\n");
		return "UNKNONWN MSG";
	}
	
	pshift++; // Skip separator char to reach string
	pcurrentchar = strrchr(pshift, ENTER_CHAR);
	if(NULL != pcurrentchar)
	{
		// found new line..
		*pcurrentchar = '\0';
	}
	*pshift = '\0'; // terminate the message before the shift id
	pshift++;
	shift = atoi(pshift);
	memset(RotatedMsg,0,sizeof(RotatedMsg));
	memset(DecodedMsg,0,sizeof(DecodedMsg));

	pcurrentchar = msg; 
	pcurrentend  = pcurrentchar;
	index = 0;
	while(pcurrentchar != NULL && pcurrentchar < pshift && '\0' != *pcurrentchar)
	{
		while(NULL != pcurrentend && '\0' != pcurrentend)
		{
			// Found space
			if (SEPERATOR_CHAR == *pcurrentend)
			{
				// found space.. terminate string.
				*pcurrentend = '\0';
				RotatedMsg[index++] = ReverseLookup(pcurrentchar);
				pcurrentend++; // Skip space;
				pcurrentchar = pcurrentend; // Jump to next code word. 
				break;
			}

			pcurrentend++; // Move end pointer to space.
		}
	}
	// Apply shift
	for(i = 0; i < index; i++)
	{
		ch = RotatedMsg[i];
		if(ENTER_CHAR == ch)
		{
			continue;
		}
		if(ch >= 'a' && ch <= 'z')
		{
			// within range.
			// apply shift.
			ch -= shift;

			// Check if roll over in post shift operation.
			if(ch < 'a')
			{
				ch = 'z' - ('a' - ch);
			}else if (ch > 'z')
			{
				ch = 'a' - ('z' - ch) - 1;
			}
		} else
		{
			ch = ch;
		}
		DecodedMsg[i] = ch; 
	}

	return DecodedMsg;
}


int main(int argc, char **argv)
{
	char Msg[SIZE_OF_MSG];
	char choice,temp;
	int shift;
	int continue_execution;
	continue_execution = 1;
	memset(Msg,0,SIZE_OF_MSG);
	
	do
	{
		printf("Enter your choice (encode->e), (decode->d) (exit ->x): ");
		scanf("%c",&choice);
		switch (choice)
		{
			case 'e':
			{
				scanf("%c",&temp); // flush buffer
				printf("Enter Message to Encode: ");
				memset(Msg,0,sizeof(Msg));
				fgets(Msg, sizeof(Msg), stdin);
				printf("Enter shift: ", shift);
				scanf("%d",&shift);
				printf("Encoded Msg: %s\n",CryptEncode(Msg, shift));

			}break;
			case 'd':
			{
				scanf("%c",&temp); // flush buffer
				printf("Enter Message To Be Decoded: ");
				memset(Msg,0,sizeof(Msg));
				fgets(Msg, sizeof(Msg), stdin);
		
				printf("Decoded Msg: %s\n",CryptDecode(Msg));
	
			}break;
			case 'x':
			{
				scanf("%c",&temp); // flush buffer
				continue_execution = 0;
				break;
			}break;
			default:
			{
				continue;
			}break;
		};
	
	}while(continue_execution != 0);
	return 0;
}


