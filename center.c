void center( char* begin, char* text, int col, char pad, char* end )
{

	int length = strlen(text);
	int i, j = 0.5 * ( col - length - 2 );

	printf("%s", begin);
	for ( i = 0; i < j; i++ ) printf("%c",pad);
	if ( *text != '\0' ) printf(" %s ", text);
	else printf("%c%c", pad, pad);
	j += 2 + length;
	for ( i = j; i < col; i++ ) printf("%c",pad);
	printf("%s", end);

}
