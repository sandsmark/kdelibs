#include <qstring.h>
#include <qfile.h>
#include <qcstring.h>

void dcopidlParse( const char *_code );

int idl_line_no;

int main( int argc, char** argv )
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage : dcopidl input_file\n");
        return -1;
    }
    QFile file( argv[1] );
    if ( !file.open( IO_ReadOnly ) )
    {
        fprintf(stderr, "Can't open input file\n");
        return -1;
    }
    
    QByteArray arr = file.readAll();
    int len = arr.size();
    arr.resize( len + 1 );
    arr[ len ] = 0;
    
    dcopidlParse( arr.data() );
		     
    file.close();
    return 0;
}
