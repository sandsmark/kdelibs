//
// KDE HTML Widget
//

#include <kurl.h>

#include "htmlobj.h"
#include "htmlclue.h"
#include "ampseq.h"
#include "html.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <qimage.h>
#include <qdrawutl.h>

//#define CLUE_DEBUG

//-----------------------------------------------------------------------------

HTMLTableCell::HTMLTableCell( int _x, int _y, int _max_width, int _percent,
	int rs, int cs, int pad )
	 : HTMLClueV( _x, _y, _max_width, _percent )
{
	rspan = rs;
	cspan = cs;
	padding = pad;
}

void HTMLTableCell::setMaxWidth( int _max_width )
{
	HTMLObject *obj;

	// We allow fixed width cells to be resized in a table
	width = max_width = _max_width;
/*
    if ( percent > 0 )
		width = _max_width * percent / 100;
	else if ( !isFixedWidth() )
		width = max_width;
*/
	for ( obj = list.first(); obj != 0L; obj = list.next() )
	{
		obj->setMaxWidth( width );
	}
}

bool HTMLTableCell::print( QPainter *_painter, int _x, int _y, int _width,
	int _height, int _tx, int _ty, bool toPrinter )
{
	if ( _y + _height < y - getAscent() || _y > y )
		return false;
    
	if ( !isPrinting() )
		return false;
    
 	if ( bg.isValid() )
	{
		int top = _y - ( y - getAscent() );
		int bottom = top + _height;
		if ( top < -padding )
			top = -padding;
		if ( bottom > getAscent() + padding )
			bottom = getAscent() + padding;

 		QBrush brush( bg );
		_painter->fillRect( _tx + x - padding, _ty + y - ascent + top,
			getMaxWidth() + padding * 2, bottom - top, brush );
	}

	return HTMLClueV::print( _painter, _x, _y, _width, _height, _tx, _ty, toPrinter );
}

//-----------------------------------------------------------------------------

HTMLTable::HTMLTable( int _x, int _y, int _max_width, int _width, int _percent,
	int _padding, int _spacing, int _border ) : HTMLObject()
{
    x = _x;
    y = _y;
    max_width = _max_width;
	width = _width;
	percent = _percent;
	padding = _padding;
	spacing = _spacing;
	border  = _border;
	caption = NULL;

	setFixedWidth( false );
	row = 0;
	col = 0;

	totalCols = 1;	// this should be expanded to the maximum number of cols
					// by the first row parsed
	totalRows = 5;	// allocate five rows initially

	cells = new HTMLTableCell ** [totalRows];

	for ( unsigned int r = 0; r < totalRows; r++ )
	{
		cells[r] = new HTMLTableCell * [totalCols];
		memset( cells[r], 0, totalCols * sizeof( HTMLTableCell * ) );
	}

	if ( percent > 0 )
		width = max_width * percent / 100;
	else if ( width == 0 )
		width = max_width;
	else
		setFixedWidth( TRUE );
}

void HTMLTable::startRow()
{
	col = 0;
}

void HTMLTable::addCell( HTMLTableCell *cell )
{
	while ( col < totalCols && cells[row][col] != NULL )
		col++;
	setCells( row, col, cell );

	col++;
}

void HTMLTable::endRow()
{
	if ( col )
		row++;
}

void HTMLTable::setCells( unsigned int r, unsigned int c, HTMLTableCell *cell )
{
	unsigned int endRow = r + cell->rowSpan();
	unsigned int endCol = c + cell->colSpan();

	if ( endCol > totalCols )
		addColumns( endCol - totalCols );

	if ( endRow >= totalRows )
		addRows( endRow - totalRows + 10 );

	for ( ; r < endRow; r++ )
	{
		for ( unsigned int tc = c; tc < endCol; tc++ )
		{
			cells[r][tc] = cell;
		}
	}
}

void HTMLTable::addRows( int num )
{
	HTMLTableCell ***newRows = new HTMLTableCell ** [totalRows + num];
	memcpy( newRows, cells, totalRows * sizeof(HTMLTableCell **) );
	delete [] cells;
	cells = newRows;

	for ( unsigned int r = totalRows; r < totalRows + num; r++ )
	{
		cells[r] = new HTMLTableCell * [totalCols];
		memset( cells[r], 0, totalCols * sizeof( HTMLTableCell * ) );
	}

	totalRows += num;
}

void HTMLTable::addColumns( int num )
{
	HTMLTableCell **newCells;

	for ( unsigned int r = 0; r < totalRows; r++ )
	{
		newCells = new HTMLTableCell * [totalCols+num];
		memcpy( newCells, cells[r], totalCols * sizeof( HTMLTableCell * ) );
		memset( newCells + totalCols, 0, num * sizeof( HTMLTableCell * ) );
		delete [] cells[r];
		cells[r] = newCells;
	}

	totalCols += num;
}

void HTMLTable::endTable()
{
	calcColumnWidths();
}

void HTMLTable::calcAbsolutePos( int _x, int _y )
{
	int lx = _x + x;
	int ly = _y + y - ascent;
	HTMLTableCell *cell;

 	unsigned int r, c;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->calcAbsolutePos( lx, ly );
		}
	}
}

HTMLAnchor* HTMLTable::findAnchor( const char *_name, QPoint *_p )
{
    HTMLAnchor *ret;
	HTMLTableCell *cell;

    _p->setX( _p->x() + x );
    _p->setY( _p->y() + y - ascent );
 
 	unsigned int r, c;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			ret = cell->findAnchor( _name, _p );
			if ( ret != NULL )
				return ret;
		}
	}
	
    _p->setX( _p->x() - x );
    _p->setY( _p->y() - y + ascent );

	return NULL;
}

void HTMLTable::reset()
{
	unsigned int r, c;
	HTMLTableCell *cell;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->reset();
		}
	}
}

void HTMLTable::calcSize( HTMLClue * )
{
	unsigned int r, c;
	HTMLTableCell *cell;

	// Attempt to get sensible cell widths
	optimiseCellWidth();

	// set cell widths and then calculate cell sizes
	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cell == cells[r+1][c] )
				continue;

			cell->setMaxWidth( columnOpt[c+1] -
				 columnOpt[ c-cell->colSpan()+1 ] - spacing -
				 padding - padding - 1 );
			cell->calcSize( NULL );
		}
	}

	if ( caption )
	{
		caption->setMaxWidth( columnOpt[ totalCols ] + border );
		caption->calcSize();
		if ( capAlign == HTMLClue::Top )
			caption->setPos( 0, caption->getAscent() );
	}

	// We have the cell sizes now, so calculate the vertical positions
	calcRowHeights();

	// set cell positions
	for ( r = 0; r < row; r++ )
	{
		int cellHeight;

		ascent = rowHeights[r+1] - padding - spacing;
		if ( caption && capAlign == HTMLClue::Top )
			ascent += caption->getHeight();

		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cell == cells[r+1][c] )
				continue;

			cell->setPos( columnOpt[ c-cell->colSpan() + 1 ] + padding,
				ascent - cell->getDescent() );

			cellHeight = rowHeights[r+1] - rowHeights[r-cell->rowSpan()+1] -
				padding - padding - spacing;
			cell->setMaxAscent( cellHeight );
		}
	}

	if ( caption && capAlign == HTMLClue::Bottom )
		caption->setPos( 0, rowHeights[ row ] + border + caption->getAscent() );

	width = columnOpt[ totalCols ] + border;
	ascent = rowHeights[ row ] + border;
	if ( caption )
		ascent += caption->getHeight();
}

// Both the minimum and preferred column sizes are calculated here.
// The hard part is choosing the actual sizes based on these two.
void HTMLTable::calcColumnWidths()
{
	unsigned int r, c;
	int borderExtra = border ? 1 : 0;

	colType.resize( totalCols+1 );
	colType.fill( Variable );

	columnPos.resize( totalCols+1 );
	columnPos[0] = border + spacing;

	columnPrefPos.resize( totalCols+1 );
	columnPrefPos[0] = border + spacing;

	colSpan.resize( totalCols+1 );
	colSpan.fill( 1 );

	for ( c = 0; c < totalCols; c++ )
	{
		columnPos[c+1] = 0;
		columnPrefPos[c+1] = 0;

		for ( r = 0; r < row; r++ )
		{
			HTMLTableCell *cell = cells[r][c];
			int colPos;

			if ( c < totalCols - 1 && cells[r][c+1] == cell )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			if (NULL == cell)
			        continue; 

/*
			// calculate minimum pos
			if ( cell->isFixedWidth() )
			{
				colPos = columnPos[ c - cell->colSpan() + 1 ] +
					cell->getWidth() + padding +
					padding + spacing + borderExtra;
			}
			else
			{
*/
				colPos = columnPos[ c - cell->colSpan() + 1 ] +
						cell->calcMinWidth() + padding + padding + spacing +
						borderExtra;
//			}
			if ( columnPos[c + 1] < colPos )
				columnPos[c + 1] = colPos;

			if ( colType[c + 1] != Variable )
				continue;

			// calculate preferred pos

			if ( cell->getPercent() > 0 )
			{
				colPos = columnPrefPos[ c - cell->colSpan() + 1 ] +
					( max_width * cell->getPercent() / 100 ) + padding +
					padding + spacing + borderExtra + 1;
				colType[c + 1] = Percent;
				colSpan[c + 1] = cell->colSpan();
				columnPrefPos[c + 1] = colPos;
			}
			else if ( cell->isFixedWidth() )
			{
				colPos = columnPrefPos[ c - cell->colSpan() + 1 ] +
					cell->getWidth() + padding +
					padding + spacing + borderExtra + 1;
				colType[c + 1] = Fixed;
				colSpan[c + 1] = cell->colSpan();
				columnPrefPos[c + 1] = colPos;
			}
			else
			{
				colPos = columnPrefPos[ c - cell->colSpan() + 1 ] +
					cell->calcPreferredWidth() + padding + padding +
					spacing + borderExtra + 1;
				if ( columnPrefPos[c + 1] < colPos )
					columnPrefPos[c + 1] = colPos;
			}

			if ( columnPrefPos[c + 1] < columnPos[c + 1] )
				columnPrefPos[c + 1] = columnPos[c + 1];
		}
	}
}

// Use the minimum and preferred cell widths to produce an optimum
// cell spacing.  When this has been done columnOpt contains
// the optimum cell widths.
void HTMLTable::optimiseCellWidth()
{
	unsigned int c, c1;
	int tableWidth = width - border;
	int totalPref = 0, totalMin = 0, totalPc = 0;

	columnOpt = columnPos.copy();

	if ( ( ( percent > 0 || isFixedWidth() ) &&
		tableWidth > columnPos[ totalCols ] ) ||
		columnPrefPos[totalCols] > tableWidth )
	{
		int addSize;

		// satisfy fixed width cells
		for ( c = 1; c <= totalCols; c++ )
		{
			int prefWidth = columnPrefPos[c] - columnPrefPos[c-colSpan[c]];
			int minWidth = columnPos[c] - columnPos[c-colSpan[c]];

			if ( colType[c] == Fixed && prefWidth > minWidth )
			{
				addSize = prefWidth - minWidth;

				for ( c1 = c; c1 <= totalCols; c1++ )
					columnOpt[c1] += addSize;
			}

			if ( colType[c] == Percent && prefWidth > minWidth )
				totalPc += ( prefWidth - minWidth );
		}

		int extra = tableWidth - columnOpt[totalCols];

		// add space to percent width columns
		if ( extra > 0 )
		{
			if ( extra > totalPc )
				extra = totalPc;
			for ( c = 1; c <= totalCols; c++ )
			{
				int prefWidth = columnPrefPos[c] - columnPrefPos[c-colSpan[c]];
				int minWidth = columnOpt[c] - columnOpt[c-colSpan[c]];

				if ( colType[c] != Percent )
					continue;

				if ( prefWidth > minWidth )
				{
					addSize = (prefWidth-minWidth) * extra / totalPc;
					for ( c1 = c; c1 <= totalCols; c1++ )
						columnOpt[c1] += addSize;
				}
			}
		}
		
		extra = tableWidth - columnOpt[totalCols];

		// add space to variable width columns
		if ( extra > 0 )
		{
			QArray<int> prefWidth( totalCols+1 );
			QArray<int> minWidth( totalCols+1 );

			for ( c = 1; c <= totalCols; c++ )
			{
				if ( colType[c] != Variable )
					continue;

				prefWidth[c] = columnPrefPos[c] - columnPrefPos[c-colSpan[c]];
				minWidth[c] = columnOpt[c] - columnOpt[c-colSpan[c]];

				if ( prefWidth[c] > tableWidth )
					prefWidth[c] = tableWidth;
				if ( prefWidth[c] > minWidth[c] )
				{
					totalPref += prefWidth[c];
					totalMin += minWidth[c];
				}
			}
			for ( c = 1; c <= totalCols; c++ )
			{
				if ( colType[c] != Variable )
					continue;

				if ( prefWidth[c] > minWidth[c] )
				{
					addSize = prefWidth[c] * extra / totalPref;

					if ( percent <= 0 && !isFixedWidth() &&
						minWidth[c] + addSize > prefWidth[c] )
					{
						addSize = prefWidth[c] - minWidth[c];
					}

					for ( c1 = c; c1 <= totalCols; c1++ )
						columnOpt[c1] += addSize;
				}
			}
		}
	}
	else
	{
		columnOpt = columnPrefPos.copy();
	}
}

void HTMLTable::calcRowHeights()
{
	unsigned int r, c;
	int borderExtra = border ? 1 : 0;
	HTMLTableCell *cell;

	rowHeights.resize( row+1 );
	rowHeights[0] = border + spacing;

	for ( r = 0; r < row; r++ )
	{
		rowHeights[r+1] = 0;
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			int rowPos = rowHeights[ r - cell->rowSpan() + 1 ] +
				cell->getHeight() + padding + padding + spacing + borderExtra;

			if ( rowPos > rowHeights[r+1] )
				rowHeights[r+1] = rowPos;
		}
	}
}

int HTMLTable::calcMinWidth()
{
	return columnPos[totalCols] + border;
}

int HTMLTable::calcPreferredWidth()
{
	return columnPrefPos[totalCols] + border;
}

void HTMLTable::setMaxWidth( int _max_width )
{
	max_width = _max_width;

	if ( percent > 0 )
		width = max_width * percent / 100;
	else if ( !isFixedWidth() )
		width = max_width;

	calcColumnWidths();
}

void HTMLTable::setMaxAscent( int _a )
{
	ascent = _a;
}

HTMLObject *HTMLTable::checkPoint( int _x, int _y )
{
	unsigned int r, c;
    HTMLObject *obj;
	HTMLTableCell *cell;

    if ( _x < x || _x > x + width || _y > y + descent || _y < y - ascent)
	return 0L;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			if ((obj = cell->checkPoint( _x-x, _y-(y - ascent) )) != 0L)
				return obj;
		}
	}

	return NULL;
}

void HTMLTable::selectByURL( QPainter *_painter, const char *_url, bool _select, int _tx, int _ty )
{
	unsigned int r, c;
	HTMLTableCell *cell;

    _tx += x;
    _ty += y - ascent;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->selectByURL( _painter, _url, _select, _tx, _ty );
		}
	}
}

void HTMLTable::select( QPainter *_painter, QRegExp& _pattern, bool _select, int _tx, int _ty )
{
	unsigned int r, c;
	HTMLTableCell *cell;

    _tx += x;
    _ty += y - ascent;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->select( _painter, _pattern, _select, _tx, _ty );
		}
	}
}

void HTMLTable::select( QPainter *_painter, bool _select, int _tx, int _ty )
{
	unsigned int r, c;
	HTMLTableCell *cell;

    _tx += x;
    _ty += y - ascent;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->select( _painter, _select, _tx, _ty );
		}
	}
}

void HTMLTable::select( QPainter *_painter, QRect & _rect, int _tx, int _ty )
{
	unsigned int r, c;
	HTMLTableCell *cell;

    _tx += x;
    _ty += y - ascent;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->select( _painter, _rect, _tx, _ty );
		}
	}
}

void HTMLTable::select( bool _select )
{
	unsigned int r, c;
	HTMLTableCell *cell;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->select( _select );
		}
	}
}

bool HTMLTable::selectText( QPainter *_painter, int _x1, int _y1,
	int _x2, int _y2, int _tx, int _ty )
{
	bool isSel = false;
	unsigned int r, c;
	HTMLTableCell *cell;

	_tx += x;
	_ty += y - ascent;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			if ( _y1 < y - ascent && _y2 > y )
			{
				isSel |= cell->selectText( _painter, 0, _y1 - ( y - ascent ),
					width + 1, _y2 - ( y - ascent ), _tx, _ty );
			}
			else if ( _y1 < y - ascent )
			{
				isSel |= cell->selectText( _painter, 0, _y1 - ( y - ascent ),
					_x2 - x, _y2 - ( y - ascent ), _tx, _ty );
			}
			else if ( _y2 > y )
			{
				isSel |= cell->selectText( _painter, _x1 - x,
					_y1 - ( y - ascent ), width + 1, _y2 - ( y - ascent ),
					_tx, _ty );
			}
			else if ( (_x1 - x < cell->getXPos() + cell->getWidth() &&
					_x2 - x > cell->getXPos() ) )
			{
				isSel |= cell->selectText( _painter, _x1 - x,
					_y1 - ( y - ascent ), _x2 - x, _y2 - ( y - ascent ),
					_tx, _ty );
			}
			else
			{
				cell->selectText( _painter, 0, 0, 0, 0, _tx, _ty );
			}
		}
	}

	return isSel;
}

void HTMLTable::getSelected( QStrList &_list )
{
	unsigned int r, c;
	HTMLTableCell *cell;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->getSelected( _list );
		}
	}
}

void HTMLTable::getSelectedText( QString &_str )
{
	unsigned int r, c;
	HTMLTableCell *cell;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;

			cell->getSelectedText( _str );
		}
	}
}
int HTMLTable::findPageBreak( int _y )
{
	if ( _y > y )
		return -1;

	unsigned int r, c;
	HTMLTableCell *cell;
	int pos, minpos = 0x7FFFFFFF;
	QArray<bool> colsDone( totalCols );
	colsDone.fill( false );

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;

			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;
			if ( colsDone[c] )
				continue;

			pos = cell->findPageBreak( _y - ( y - ascent ) );
			if ( pos >= 0 && pos < minpos )
			{
				colsDone[c] = true;
				minpos = pos;
			}
		}

	}

	if ( minpos != 0x7FFFFFFF )
		return ( minpos + y - ascent );

	return -1;
}

bool HTMLTable::print( QPainter *_painter, int _x, int _y, int _width, int _height, int _tx, int _ty, bool toPrinter )
{
    if ( _y + _height < y - getAscent() || _y > y )
	return false;
    
    if ( !isPrinting() )
	return false;
    
    _tx += x;
    _ty += y - ascent;
    
	unsigned int r, c;
	HTMLTableCell *cell;
	QArray<bool> colsDone( totalCols );
	colsDone.fill( false );

	if ( caption )
	{
		caption->print( _painter, _x - x, _y - (y - ascent),
			_width, _height, _tx, _ty, toPrinter );
	}

	// draw the cells
	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;
			if ( colsDone[c] )
				continue;
			if ( cell->print( _painter, _x - x, _y - (y - ascent),
					_width, _height, _tx, _ty, toPrinter ) )
				colsDone[c] = true;
		}
	}

	// draw the border - needs work to print to printer
	if ( border && !toPrinter )
	{
		int capOffset = 0;
		if ( caption && capAlign == HTMLClue::Top )
			capOffset = caption->getHeight();
		QColorGroup colorGrp( black, lightGray, white, darkGray, gray,
			black, white );
		qDrawShadePanel( _painter, _tx, _ty + capOffset, width,
			rowHeights[row] + border, colorGrp, false, border );

		// draw borders around each cell
		for ( r = 0; r < row; r++ )
		{
			for ( c = 0; c < totalCols; c++ )
			{
				if ( ( cell = cells[r][c] ) == NULL )
					continue;
				if ( c < totalCols - 1 && cell == cells[r][c+1] )
					continue;
				if ( r < row - 1 && cells[r+1][c] == cell )
					continue;

				qDrawShadePanel(_painter,
					_tx + columnOpt[c-cell->colSpan()+1],
					_ty + rowHeights[r-cell->rowSpan()+1] + capOffset,
					columnOpt[c+1] - columnOpt[c - cell->colSpan() + 1] -
					spacing,
					rowHeights[r+1] - rowHeights[r-cell->rowSpan()+1]-spacing,
					colorGrp, TRUE, 1 );
			}
		}
	}

	for ( c = 0; c < totalCols; c++ )
	{
		if ( colsDone[c] == true )
			return true;
	}

	return false;
}

void HTMLTable::print( QPainter *_painter, int _tx, int _ty )
{
    print( _painter, 0, 0, 0xFFFF, 0xFFFF, _tx, _ty );
}

HTMLTable::~HTMLTable()
{
	unsigned int r, c;
	HTMLTableCell *cell;

	for ( r = 0; r < row; r++ )
	{
		for ( c = 0; c < totalCols; c++ )
		{
			if ( ( cell = cells[r][c] ) == NULL )
				continue;
			if ( c < totalCols - 1 && cell == cells[r][c+1] )
				continue;
			if ( r < row - 1 && cells[r+1][c] == cell )
				continue;
			delete cell;
		}

		delete [] cells[r];
	}

	delete [] cells;

	delete caption;
}

//-----------------------------------------------------------------------------

HTMLClue::HTMLClue( int _x, int _y, int _max_width, int _percent )
	 : HTMLObject()
{
    x = _x;
    y = _y;
    max_width = _max_width;
	percent = _percent;
    valign = Bottom;
    halign = Left;
    list.setAutoDelete( TRUE );

	if ( percent > 0 )
	{
		width = max_width * percent / 100;
		setFixedWidth( false );
	}
	else if ( percent < 0 )
	{
		width = max_width;
		setFixedWidth( false );
	}
	else
		width = max_width;

	prevCalcObj = -1;
}

void HTMLClue::reset()
{
	HTMLObject *obj;

	for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->reset();

	prevCalcObj = -1;
}

void HTMLClue::calcAbsolutePos( int _x, int _y )
{
	HTMLObject *obj;

	int lx = _x + x;
	int ly = _y + y - ascent;

	for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->calcAbsolutePos( lx, ly );
}

HTMLAnchor* HTMLClue::findAnchor( const char *_name, QPoint *_p )
{
    HTMLObject *obj;
    HTMLAnchor *ret;

    _p->setX( _p->x() + x );
    _p->setY( _p->y() + y - ascent );
    
    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	ret = obj->findAnchor( _name, _p );
	if ( ret != 0L )
	    return ret;
    }
    
    _p->setX( _p->x() - x );
    _p->setY( _p->y() - y + ascent );

    return 0L;
}

void HTMLClue::getSelected( QStrList &_list )
{
    HTMLObject *obj;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->getSelected( _list );
    }
}

void HTMLClue::getSelectedText( QString &_str )
{
    HTMLObject *obj;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->getSelectedText( _str );
    }
}

void HTMLClue::select( bool _select )
{
    HTMLObject *obj;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->select( _select );
    }
}

void HTMLClue::select( QPainter *_painter, QRegExp& _pattern, bool _select, int _tx, int _ty )
{
    HTMLObject *obj;

    _tx += x;
    _ty += y - ascent;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->select( _painter, _pattern, _select, _tx, _ty );
    }
}

void HTMLClue::select( QPainter *_painter, bool _select, int _tx, int _ty )
{
    HTMLObject *obj;

    _tx += x;
    _ty += y - ascent;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->select( _painter, _select, _tx, _ty );
    }
}

void HTMLClue::select( QPainter *_painter, QRect & _rect, int _tx, int _ty )
{
    HTMLObject *obj;

    QRect r( x + _tx, y - ascent + _ty, width, ascent + descent );

    _tx += x;
    _ty += y - ascent;
    
    if ( _rect.contains( r ) )
      for ( obj = list.first(); obj != 0L; obj = list.next() )
      {
	obj->select( _painter, TRUE, _tx, _ty );
      }
    else if ( !_rect.intersects( r ) )
      for ( obj = list.first(); obj != 0L; obj = list.next() )
      {
	obj->select( _painter, FALSE, _tx, _ty );
      }
    else
      for ( obj = list.first(); obj != 0L; obj = list.next() )
      {
	obj->select( _painter, _rect, _tx, _ty );
      }
}

void HTMLClue::selectByURL( QPainter *_painter, const char *_url, bool _select, int _tx, int _ty )
{
    HTMLObject *obj;

    // _painter->translate( (float)x, (float)( y - ascent ) );
    _tx += x;
    _ty += y - ascent;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->selectByURL( _painter, _url, _select, _tx, _ty );
    }

    // _painter->translate( (float)-x, (float)-( y - ascent ) );
}

bool HTMLClue::selectText( QPainter *_painter, int _x1, int _y1,
	int _x2, int _y2, int _tx, int _ty )
{
    HTMLObject *obj;
    bool isSel = false;

    _tx += x;
    _ty += y - ascent;

    for ( obj = list.first(); obj != NULL; obj = list.next() )
    {
	isSel |= obj->selectText( _painter, _x1 - x, _y1 - ( y - ascent ),
		_x2 - x, _y2 - ( y - ascent ), _tx, _ty );
    }

    return isSel;
}

HTMLObject* HTMLClue::checkPoint( int _x, int _y )
{
    HTMLObject *obj;
    HTMLObject *obj2;

    if ( _x < x || _x > x + width || _y > y + descent || _y < y - ascent)
	return 0L;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ((obj2 = obj->checkPoint( _x - x, _y - (y - ascent) )) != 0L)
	    return obj2;
    }

    return 0L;
}

void HTMLClue::calcSize( HTMLClue * )
{
    HTMLObject *obj;

    // If we have already called calcSize for the children, then just
    // continue from the last object done in previous call.
    if ( prevCalcObj >= 0 )
	obj = list.at( prevCalcObj );	// this will always find an object
    else
    {
//	x = y = ascent = 0;
	ascent = 0;
	obj = list.first();
    }

    while ( obj != NULL )
    {
	obj->calcSize( this );
	obj = list.next();
    }

    // remember this object so that we can start from here next time
    // we are called.
    list.last();
    prevCalcObj = list.at();
}

void HTMLClue::recalcBaseSize( QPainter *_painter )
{
    HTMLObject *obj;

    for ( obj = list.first(); obj != NULL; obj = list.next() )
	obj->recalcBaseSize( _painter );
}

int HTMLClue::calcMinWidth()
{
    HTMLObject *obj;
    int minWidth = 0;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	int w = obj->calcMinWidth();
	if ( w > minWidth )
	    minWidth = w;
    }

    if ( isFixedWidth() )
	return width > minWidth ? width : minWidth;

    return minWidth;
}

int HTMLClue::calcPreferredWidth()
{
    if ( isFixedWidth() )
	return width;

    HTMLObject *obj;
    int prefWidth = 0;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	int w = obj->calcPreferredWidth();
	if ( w > prefWidth )
	    prefWidth = w;
    }

    return prefWidth;
}

void HTMLClue::setMaxAscent( int _a )
{
    HTMLObject *obj;

    if ( valign == VCenter )
    {
	for ( obj = list.first(); obj != 0L; obj = list.next() )
	    obj->setYPos( obj->getYPos() + ( _a - ascent )/2 );
    }
    else if ( valign == Bottom )
    {
	for ( obj = list.first(); obj != 0L; obj = list.next() )
	    obj->setYPos( obj->getYPos() + _a - ascent );
    }

    ascent = _a;
}

int HTMLClue::findPageBreak( int _y )
{
    if ( _y > y )
	return -1;

    HTMLObject *obj;
    int pos;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( !obj->isAligned() )
	{
	    pos = obj->findPageBreak( _y - ( y - ascent ) );
	    if ( pos >= 0 )
		return ( pos + y - ascent );
	}
    }

    return -1;
}

bool HTMLClue::print( QPainter *_painter, int _x, int _y, int _width, int _height, int _tx, int _ty, bool toPrinter )
{
    if ( _y + _height < y - getAscent() || _y > y )
	return false;
    
    if ( !isPrinting() )
	return false;
    
    HTMLObject *obj;

    _tx += x;
    _ty += y - ascent;

#ifdef CLUE_DEBUG
    // draw rectangles around clues - for debugging
    QBrush brush;
    _painter->setBrush( brush );
    _painter->drawRect( _tx, _ty, width, getHeight() );
#endif

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( !obj->isAligned() )
	{
	    if ( obj->print( _painter, _x - x, _y - (y - getHeight()),
			_width, _height, _tx, _ty, toPrinter ) && toPrinter )
	    return true;
	}
    }

    return false;
}

void HTMLClue::print( QPainter *_painter, int _tx, int _ty )
{
    print( _painter, 0, 0, 0xFFFF, 0xFFFF, _tx, _ty );
}

void HTMLClue::print( QPainter *_painter, int _x, int _y, int _width, int _height, int _tx, int _ty, HTMLObject *_obj )
{
    if ( _y + _height < y - getAscent() || _y > y )
	return;
    
    if ( !isPrinting() )
	return;
    
    HTMLObject *obj;

    _tx += x;
    _ty += y - ascent;
    
    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( obj == _obj )
	{
	    obj->print( _painter, _x - x, _y - (y - getHeight()), _width,	
		    _height, _tx, _ty );
	    return;
	}
    }
}

//-----------------------------------------------------------------------------

void HTMLClueV::reset()
{
    HTMLClue::reset();

    alignLeftList.clear();
    alignRightList.clear();
}

void HTMLClueV::setMaxWidth( int _max_width )
{
    HTMLObject *obj;

    if ( !isFixedWidth() )
    {
	max_width = _max_width;
	if ( percent > 0 )
	    width = _max_width * percent / 100;
	else
	    width = max_width;
    }

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->setMaxWidth( width );
    }
}

HTMLObject* HTMLClueV::checkPoint( int _x, int _y )
{
    HTMLObject *obj2;

    if ( ( obj2 = HTMLClue::checkPoint( _x, _y ) ) != NULL )
	    return obj2;

    if ( _x < x || _x > x + width || _y > y + descent || _y < y - ascent)
	return 0L;

    HTMLClueAligned *clue;
    for ( clue=alignLeftList.first(); clue!=NULL; clue=alignLeftList.next() )
    {
	if ((obj2 = clue->checkPoint( _x - x - clue->parent()->getXPos(),
		 _y - (y - ascent) - ( clue->parent()->getYPos() -
		 clue->parent()->getAscent() ) )) != 0L)
	    return obj2;
    }
    for ( clue=alignRightList.first(); clue!=NULL; clue=alignRightList.next() )
    {
	if ((obj2 = clue->checkPoint( _x - x - clue->parent()->getXPos(),
		 _y - (y - ascent) - ( clue->parent()->getYPos() -
		 clue->parent()->getAscent() ) )) != 0L)
	    return obj2;
    }

    return 0L;
}

void HTMLClueV::calcSize( HTMLClue *parent )
{
	HTMLObject *obj;
	int lmargin = parent ? parent->getLeftMargin( getYPos() ) : 0;

	// If we have already called calcSize for the children, then just
	// continue from the last object done in previous call.
	if ( prevCalcObj >= 0 )
	{
	    obj = list.at( prevCalcObj );  // This will always find an object.
	    ascent -= obj->getHeight();	   // Otherwise it will be added twice.
	}
	else
	{
	    ascent = descent = 0;
	    obj = list.first();
	}

	while ( obj != NULL )
	{
	    // Set an initial ypos so that the alignment stuff knows where
	    // the top of this object is
	    obj->setYPos( ascent );
	    obj->calcSize( this );
	    if ( obj->getWidth() > width )
		    width = obj->getWidth();
	    ascent += obj->getHeight();
	    obj->setPos( lmargin, ascent - obj->getDescent() );
	    obj = list.next();
	}

	// remember this object so that we can start from here next time
	// we are called.
	list.last();
	prevCalcObj = list.at();
/*
	if (max_width != 0 && width < max_width)
		width = max_width;
*/
	if ( halign == HCenter )
	{
	    for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->setXPos( lmargin + (width - obj->getWidth()) / 2 );
	}
	else if ( halign == Right )
	{
	    for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->setXPos( lmargin + width - obj->getWidth() );
	}

	HTMLClueAligned *clue;
	for ( clue=alignLeftList.first(); clue!=NULL; clue=alignLeftList.next() )
	{
	    if ( clue->getYPos() + clue->parent()->getYPos() > ascent )
		ascent = clue->getYPos() + clue->parent()->getYPos();
	}
	for ( clue=alignRightList.first(); clue!=NULL; clue=alignRightList.next() )
	{
	    if ( clue->getYPos() + clue->parent()->getYPos() > ascent )
		ascent = clue->getYPos() + clue->parent()->getYPos();
	}
}

bool HTMLClueV::print( QPainter *_painter, int _x, int _y, int _width, int _height, int _tx, int _ty, bool toPrinter )
{
    bool rv = HTMLClue::print( _painter, _x, _y, _width, _height, _tx, _ty, toPrinter );

    // print aligned objects
    if ( _y + _height < y - getAscent() || _y > y )
	return rv;
    
    if ( !isPrinting() )
	return rv;

    _tx += x;
    _ty += y - ascent;

    HTMLClueAligned *clue;

    for ( clue=alignLeftList.first(); clue != 0L; clue=alignLeftList.next() )
    {
	clue->print( _painter, _tx + clue->parent()->getXPos(),
		_ty + clue->parent()->getYPos() - clue->parent()->getAscent() );
    }

    for ( clue=alignRightList.first(); clue != 0L; clue=alignRightList.next() )
    {
	clue->print( _painter, _tx + clue->parent()->getXPos(),
		_ty + clue->parent()->getYPos() - clue->parent()->getAscent() );
    }

    return rv;
}

int HTMLClueV::getLeftMargin( int _y )
{
    int margin = 0;
    HTMLClueAligned *clue;

    for ( clue=alignLeftList.first(); clue!=NULL; clue=alignLeftList.next() )
    {
	if ( clue->getYPos() - clue->getAscent() + clue->parent()->getYPos() -
		    clue->parent()->getAscent() < _y &&
		    clue->getYPos() + clue->parent()->getYPos() -
		    clue->parent()->getAscent() > _y )
	    margin = clue->getXPos() + clue->getWidth();
    }

    return margin;
}

int HTMLClueV::getRightMargin( int _y )
{
    int margin = max_width;
    HTMLClueAligned *clue;

    for ( clue=alignRightList.first(); clue!=NULL; clue=alignRightList.next() )
    {
	if ( clue->getYPos()-clue->getAscent()+clue->parent()->getYPos() -
		clue->parent()->getAscent() < _y &&
		clue->getYPos() + clue->parent()->getYPos() -
		clue->parent()->getAscent() > _y )
	    margin = clue->getXPos();
    }

    return margin;
}

//-----------------------------------------------------------------------------

bool HTMLClueH::selectText( QPainter *_painter, int _x1, int _y1,
	int _x2, int _y2, int _tx, int _ty )
{
    HTMLObject *obj;
    bool isSel = false;
    int a = 0, d = 0;

    if ( list.isEmpty() )
	return false;

    _tx += x;
    _ty += y - ascent;

    // get max ascent and descent
    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( obj->getAscent() > a )
	    a = obj->getAscent();
	if ( obj->getDescent() > d )
	    d = obj->getDescent();
    }

    int rely1 = _y1 - ( y - ascent );
    int rely2 = _y2 - ( y - ascent );
    int ypos = list.getFirst()->getYPos();

    if ( rely1 > ypos - a && rely1 < ypos + d )
	rely1 = ypos-1;

    if ( rely2 > ypos - a && rely2 < ypos + d )
	rely2 = ypos;

    // (de)select objects
    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( obj->getObjectType() == Clue )
	    isSel |= obj->selectText( _painter, _x1 - x, _y1 - (y-ascent),
		    _x2 - x, _y2 - ( y - ascent ), _tx, _ty );
	else
	    isSel |= obj->selectText( _painter, _x1 - x, rely1,
		    _x2 - x, rely2, _tx, _ty );
    }

    return isSel;
}

void HTMLClueH::setMaxWidth( int _w )
{
    HTMLObject *obj;
    max_width = _w;

    // first calculate width minus fixed width objects
    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( obj->getPercent() == 0 )	// i.e. fixed width objects
	    _w -= obj->getWidth();
    }

    // now call setMaxWidth for variable objects
    for ( obj = list.first(); obj != 0L; obj = list.next() )
	if ( obj->getPercent() > 0 )
	    obj->setMaxWidth( _w );
}

void HTMLClueH::calcSize( HTMLClue *parent )
{
    // make sure children are properly sized
    setMaxWidth( max_width );

    HTMLClue::calcSize( this );
    
    HTMLObject *obj;
    int lmargin = parent->getLeftMargin( getYPos() );

    width = lmargin;
    descent = 0;
    ascent = 0;

    int a = 0;
    int d = 0;
    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	obj->setXPos( width );
	width += obj->getWidth();
	if ( obj->getAscent() > a )
	    a = obj->getAscent();
	if ( obj->getDescent() > d )
	    d = obj->getDescent();
    }

    ascent = a+d;

    switch ( valign )
    {
	case Top:
	    for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->setYPos( obj->getAscent() );
	    break;

	case VCenter:
	    for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->setYPos( ascent/2 );
	    break;

	default:
	    for ( obj = list.first(); obj != 0L; obj = list.next() )
		obj->setYPos( ascent - d );
    }
}

int HTMLClueH::calcMinWidth()
{
    HTMLObject *obj;
	int minWidth = 0;

	for ( obj = list.first(); obj != 0L; obj = list.next() )
		minWidth += obj->calcMinWidth();
	
	return minWidth;
}
 
int HTMLClueH::calcPreferredWidth()
{
    HTMLObject *obj;
	int prefWidth = 0;

	for ( obj = list.first(); obj != 0L; obj = list.next() )
		prefWidth += obj->calcPreferredWidth();
	
	return prefWidth;
}
    
//-----------------------------------------------------------------------------

// process one line at a time, making sure that all objects on a line
// are able to be selected if the cursor is within the maximum
// ascent and descent of the line.
//
bool HTMLClueFlow::selectText( QPainter *_painter, int _x1, int _y1,
	int _x2, int _y2, int _tx, int _ty )
{
    HTMLObject *lineEnd, *obj;
    bool isSel = false;
    int ypos, a, d, rely1, rely2;

    _tx += x;
    _ty += y - ascent;

    QListIterator<HTMLObject> line( list );
    lineEnd = line.current();
    obj = list.first();

    while ( lineEnd )
    {
	ypos = lineEnd->getYPos();
	a = lineEnd->getAscent();
	d = lineEnd->getDescent();

	while ( lineEnd && lineEnd->getYPos() == ypos )
	{
	    if ( lineEnd->getAscent() > a )
		a = lineEnd->getAscent();
	    if ( lineEnd->getDescent() > d )
		d = lineEnd->getDescent();
	    lineEnd = ++line;
	}

	rely1 = _y1 - ( y - ascent );
	rely2 = _y2 - ( y - ascent );

	if ( rely1 > ypos - a && rely1 < ypos + d )
	    rely1 = ypos-1;

	if ( rely2 > ypos - a && rely2 < ypos + d )
	    rely2 = ypos;

	while ( obj != lineEnd )
	{
	    if ( obj->getObjectType() == Clue )
		isSel |= obj->selectText( _painter, _x1 - x, _y1 - (y-ascent),
			_x2 - x, _y2 - ( y - ascent ), _tx, _ty );
	    else
		isSel |= obj->selectText( _painter, _x1 - x, rely1,
			_x2 - x, rely2, _tx, _ty );
	    obj = list.next();
	}
    }

    return isSel;
}

void HTMLClueFlow::getSelectedText( QString &_str )
{
    HTMLObject *obj;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( obj != list.getFirst() || !obj->isSeparator() )
	    obj->getSelectedText( _str );
    }

    if ( !list.isEmpty() && list.getLast()->isSelected() )
	_str += '\n';
}

void HTMLClueFlow::calcSize( HTMLClue *parent )
{
	HTMLClue::calcSize( this );

	HTMLObject *obj;
	int lmargin, rmargin;

	ascent = 0;
	descent = 0;
	width = 0;
	lmargin = parent->getLeftMargin( getYPos() );
	if ( indent > lmargin )
		lmargin = indent;
	rmargin = parent->getRightMargin( getYPos() );
	int w = lmargin;
	int a = 0;
	int d = 0;

	QListIterator<HTMLObject> line( list );
	QListIterator<HTMLObject> it( list );    

	for ( ; it.current() ; ++it )
	{
		obj = it.current();

		// This left or right aligned with other objects to wrap around it
		if ( obj->isAligned() )
		{
			HTMLClueAligned *c = (HTMLClueAligned *)obj;
			if ( c->getHAlign() == Left )
				parent->appendLeftAligned( c );
			else
				parent->appendRightAligned( c );
			if ( w == lmargin )
			{
				if ( c->getHAlign() == Left )
					c->setPos( lmargin,
						ascent + c->getAscent() );
				else
					c->setPos( rmargin - c->getWidth(),
						ascent + c->getAscent() );
				while ( it.current() != line.current() )
					++line;
				++line;
				lmargin = parent->getLeftMargin( getYPos() + 1 );
				if ( indent > lmargin )
					lmargin = indent;
				rmargin = parent->getRightMargin( getYPos() + 1 );
				w = lmargin;
			}
		}
		// Object exceeds max_width => new line containing only this object
		else if ( obj->getWidth() > rmargin && !obj->isSeparator() &&
			!obj->isNewline() && rmargin != 0)
		{
			if ( w != 0)
			{
				ascent += a + d;
				y += a + d;
			}

			if ( w > width )
				width = w;

			while ( line.current() != it.current() )
			{
				if ( line.current()->isAligned() )
				{
					HTMLClue *c = (HTMLClue *)line.current();
					if ( c->getHAlign() == Left )
					{
						int margin = parent->getLeftMargin( getYPos() + 1 );
						c->setPos( margin, ascent + c->getAscent() );
					}
					else
					{
						int margin = parent->getRightMargin( getYPos() + 1 );
						c->setPos( margin - c->getWidth(),
							ascent + c->getAscent() );
					}
				}
				else
				{
					line.current()->setYPos( ascent - d );
					line.current()->setMaxAscent( a );
					if ( halign == HCenter )
						line.current()->setXPos( line.current()->getXPos()+
							( rmargin - w )/2 );
					else if ( halign == Right )
						line.current()->setXPos( line.current()->getXPos()+
							( rmargin - w ) );
				}
				++line;
			}

			lmargin = parent->getLeftMargin( getYPos() + 1 );
			if ( indent > lmargin )
				lmargin = indent;
			rmargin = parent->getRightMargin( getYPos() + 1 );

			ascent += obj->getHeight();
			obj->setPos( lmargin, ascent - obj->getDescent() );
			++line;

			if ( obj->getWidth() > width )
				width = obj->getWidth();
			w = lmargin;
			d = 0;
			a = 0;
		}
		// Object fits in this line and is no newline
		else if ( w + obj->getWidth() <= rmargin && rmargin != 0 &&
			!obj->isNewline() )
		{
			if ( obj->getAscent() > a )
				a = obj->getAscent();

			if ( obj->getDescent() > d )
				d = obj->getDescent();

			obj->setXPos( w );

			// skip a space at the start of a line
			if ( w != lmargin || !obj->isSeparator() )
			{
				w += obj->getWidth();
				if ( w > width)
					width = w;
			}
		}
		// Object does not fit in the line but is smaller than max_width or
		// it is a newline
		else 
		{
			// Perhaps the newline is the only object in this line...
			if ( obj->isNewline() )
			{
				if ( obj->getAscent() > a )
					a = obj->getAscent();
				if ( obj->getDescent() > d )
					d = obj->getDescent();
			}

			ascent += a + d;
			y += a + d;

			while ( line.current() != it.current() )
			{
				if ( line.current()->isAligned() )
				{
					HTMLClue *c = (HTMLClue *)line.current();
					if ( c->getHAlign() == Left )
					{
						int margin = parent->getLeftMargin( getYPos() + 1 );
						c->setPos( margin, ascent + c->getAscent() );
					}
					else
					{
						int margin = parent->getRightMargin( getYPos() + 1 );
						c->setPos( margin - c->getWidth(),
							ascent + c->getAscent() );
					}
				}
				else
				{
					line.current()->setYPos( ascent - d );
					line.current()->setMaxAscent( a );
					if ( halign == HCenter )
						line.current()->setXPos( line.current()->getXPos()+
							( rmargin - w )/2 );
					else if ( halign == Right )
						line.current()->setXPos( line.current()->getXPos()+
							( rmargin - w ) );
				}
				++line;
			}

			lmargin = parent->getLeftMargin( getYPos() + 1 );
			if ( indent > lmargin )
				lmargin = indent;
			rmargin = parent->getRightMargin( getYPos() + 1 );

			// Do not print newlines or separators at the beginning of a line.
			if ( obj->isSeparator() || obj->isNewline() )
			{
				w = lmargin;
				a = 0;
				d = 0;
			}
			else
			{
				w = obj->getWidth() + lmargin;
				a = obj->getAscent();
				d = obj->getDescent();
				if ( w > width)
					width = w;
			}

			obj->setXPos( lmargin );
		}

	}

	if (w != 0)
	{
		ascent += a + d;
		y += a + d;

		while ( line.current() != it.current() )
		{
			if ( line.current()->isAligned() )
			{
				HTMLClue *c = (HTMLClue *)line.current();
				if ( c->getHAlign() == HTMLClue::Left )
				{
					int margin = parent->getLeftMargin( getYPos() + 1 );
					c->setPos( margin, ascent + c->getAscent() );
				}
				else
				{
					int margin = parent->getRightMargin( getYPos() + 1 );
					c->setPos( margin - c->getWidth(),
						ascent + c->getAscent() );
				}
			}
			else
			{	
				line.current()->setYPos( ascent - d );
				line.current()->setMaxAscent( a );
				if ( halign == HCenter )
					line.current()->setXPos( line.current()->getXPos()+
						( rmargin - w )/2 );
				else if ( halign == Right )
					line.current()->setXPos( line.current()->getXPos()+
						( rmargin - w ) );
			}
			++line;
		}

		if ( w > width )
			width = w;
	}    

	if ( width < max_width )
		width = max_width;
}

int HTMLClueFlow::findPageBreak( int _y )
{
	if ( _y > y )
		return -1;

	HTMLObject *obj;
	int pos, minpos, yp;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
		yp = obj->getYPos();
		minpos = yp;

		while ( obj && obj->getYPos() == yp )
		{
			if ( !obj->isAligned() )
			{
				pos = obj->findPageBreak( _y - ( y - ascent ) );
				if ( pos >= 0 && pos < minpos )
					minpos = pos;
			}
			obj = list.next();
		}
		if ( minpos != yp )
			return ( minpos + y - ascent );
    }

	return -1;
}

int HTMLClueFlow::calcMinWidth()
{
	return HTMLClue::calcMinWidth() + indent;
}

int HTMLClueFlow::calcPreferredWidth()
{
	HTMLObject *obj;
	int maxw = 0, w = 0;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
		if ( !obj->isNewline() )
		{
			if ( !obj->isAligned() )
				w += obj->calcPreferredWidth();
		}
		else
		{
			if ( w > maxw )
				maxw = w;
			w = 0;
		}
    }

	if ( w > maxw )
		maxw = w;

	return maxw + indent;
}

void HTMLClueFlow::setMaxWidth( int _max_width )
{
	max_width = _max_width;

	HTMLObject *obj;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
		obj->setMaxWidth( max_width - indent );
	}
}


//-----------------------------------------------------------------------------


void HTMLClueAligned::setMaxWidth( int _max_width )
{
	max_width = _max_width;

	HTMLObject *obj;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
		obj->setMaxWidth( max_width );
	}
}

// HTMLClueAligned behaves like a HTMLClueV
//
void HTMLClueAligned::calcSize( HTMLClue * )
{
    HTMLClue::calcSize( this );

    HTMLObject *obj;

	width = 0;
	ascent = ALIGN_BORDER;
	descent = 0;

    for ( obj = list.first(); obj != 0L; obj = list.next() )
    {
	if ( obj->getWidth() > width )
	     width = obj->getWidth();
	ascent += obj->getHeight();
	obj->setPos( ALIGN_BORDER, ascent - obj->getDescent() );
    }

	ascent += ALIGN_BORDER;
	width += (ALIGN_BORDER*2);
}

//-----------------------------------------------------------------------------

