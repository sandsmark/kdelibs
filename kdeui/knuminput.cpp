/*
 * knuminput.cpp
 *
 * Initial implementation:
 *     Copyright (c) 1997 Patrick Dowler <dowler@morgul.fsh.uvic.ca>
 *
 *     Copyright (c) 1999 Dirk A. Mueller <dmuell@gmx.net>
 *
 *  Requires the Qt widget libraries, available at no cost at
 *  http://www.troll.no/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */

#include <config.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <math.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qsize.h>
#include <qslider.h>
#include <qspinbox.h>

#include "knumvalidator.h"
#include "knuminput.h"


// -----------------------------------------------------------------------------

KNumInput::KNumInput(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    init();
}


// -----------------------------------------------------------------------------

KNumInput::KNumInput(KNumInput* below, QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    init();

    if(below) {
        m_next = below->m_next;
        m_prev = below;
        below->m_next = this;
        if(m_next)
            m_next->m_prev = this;
    }
}


// -----------------------------------------------------------------------------

void KNumInput::init()
{
    m_prev = m_next = 0;
    m_colw1 = m_colw2 = 0;

    m_label = 0;
    m_slider = 0;
    m_alignment = 0;
}


// -----------------------------------------------------------------------------

KNumInput::~KNumInput()
{
    if(m_prev)
        m_prev->m_next = m_next;

    if(m_next)
        m_next->m_prev = m_prev;
}


// -----------------------------------------------------------------------------

void KNumInput::setLabel(QString label, int a)
{
    if(label.isEmpty()) {
        delete m_label;
        m_label = 0;
        m_alignment = 0;
    }
    else {
        m_label = new QLabel(label, this, "KNumInput::QLabel");
        m_label->setAlignment((a & (~(AlignTop|AlignBottom|AlignVCenter))) | AlignVCenter);
        // if no vertical alignment set, use Top alignment
        if(!(a & (AlignTop|AlignBottom|AlignVCenter)))
           a |= AlignTop;
        m_alignment = a;
    }

    layout(true);
}


// -----------------------------------------------------------------------------

void KNumInput::layout(bool deep)
{
    int w1 = m_colw1;
    int w2 = m_colw2;

    // label sizeHint
    m_sizeLabel = (m_label ? m_label->sizeHint() : QSize(0,0));

    if(m_label && (m_alignment & AlignVCenter))
        m_colw1 = m_sizeLabel.width() + 4;
    else
        m_colw1 = 0;

    // slider sizeHint
    m_sizeSlider = (m_slider ? m_slider->sizeHint() : QSize(0, 0));

    doLayout();

    if(!deep) {
        m_colw1 = w1;
        m_colw2 = w2;
        return;
    }

    KNumInput* p = this;
    while(p) {
        p->doLayout();
        w1 = QMAX(w1, p->m_colw1);
        w2 = QMAX(w2, p->m_colw2);
        p = p->m_prev;
    }

    p = m_next;
    while(p) {
        p->doLayout();
        w1 = QMAX(w1, p->m_colw1);
        w2 = QMAX(w2, p->m_colw2);
        p = p->m_next;
    }

    p = this;
    while(p) {
        p->m_colw1 = w1;
        p->m_colw2 = w2;
        p = p->m_prev;
    }

    p = m_next;
    while(p) {
        p->m_colw1 = w1;
        p->m_colw2 = w2;
        p = p->m_next;
    }

//    qDebug("w1 %d w2 %d", w1, w2);
}


// -----------------------------------------------------------------------------

QSizePolicy KNumInput::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
}


// -----------------------------------------------------------------------------

QSize KNumInput::sizeHint() const
{
    return minimumSizeHint();
}


// -----------------------------------------------------------------------------

void KNumInput::setSteps(int minor, int major)
{
    if(m_slider)
        m_slider->setSteps( minor, major );
}


// -----------------------------------------------------------------------------

KIntSpinBox::KIntSpinBox(int lower, int upper, int step, int value, int base,
                         QWidget* parent, const char* name)
    : QSpinBox(lower, upper, step, parent, name)
{
    editor()->setAlignment(AlignRight);
    val_base = base;
    setValue(value);
}


// -----------------------------------------------------------------------------

QString KIntSpinBox::mapValueToText(int v)
{
    return QString::number(v, val_base);
}


// -----------------------------------------------------------------------------

int KIntSpinBox::mapTextToValue(bool* ok)
{
    return cleanText().toInt(ok, val_base);
}


// -----------------------------------------------------------------------------

KIntNumInput::KIntNumInput(int val, QWidget *parent, int _base, const char *name)
    : KNumInput(parent, name)
{
    init(val, _base);

}


// -----------------------------------------------------------------------------

void KIntSpinBox::setEditFocus(bool mark)
{
    editor()->setFocus();
    if(mark)
        editor()->selectAll();
}


// -----------------------------------------------------------------------------

KIntNumInput::KIntNumInput(KNumInput* below, int val, QWidget* parent,
                           int _base, const char* name)
    : KNumInput(below, parent, name)
{
    init(val, _base);
}


// -----------------------------------------------------------------------------

void KIntNumInput::init(int val, int _base)
{
    m_spin = new KIntSpinBox(INT_MIN, INT_MAX, 1, val, _base, this, "KIntNumInput::KIntSpinBox");
    m_spin->setValidator(new KIntValidator(this, _base, "KNumInput::KIntValidtr"));
    connect(m_spin, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)));

    setFocusProxy(m_spin);
}


// -----------------------------------------------------------------------------

void KIntNumInput::sliderMoved(int val)
{
    m_spin->setValue(val);
    emit valueChanged(val);
}


// -----------------------------------------------------------------------------

void KIntNumInput::setRange(int lower, int upper, int step, bool slider)
{
    m_spin->setMinValue(lower);
    m_spin->setMaxValue(upper);
    m_spin->setLineStep(step);

    if(slider) {
        m_slider = new QSlider(lower, upper, step, m_spin->value(),
                               QSlider::Horizontal, this);
        m_slider->setTickmarks(QSlider::Below);

        int major = (upper-lower)/10;
        m_slider->setSteps( step, major );
        m_slider->setTickInterval(major);
        connect(m_slider, SIGNAL(valueChanged(int)), SLOT(sliderMoved(int)));
    }
    else {
        delete m_slider;
        m_slider = 0;
    }

    layout(true);
}


// -----------------------------------------------------------------------------

void KIntNumInput::setSuffix(QString suffix)
{
    m_spin->setSuffix(suffix);

    layout(true);
}


// -----------------------------------------------------------------------------

void KIntNumInput::setPrefix(QString prefix)
{
    m_spin->setPrefix(prefix);

    layout(true);
}


// -----------------------------------------------------------------------------

void KIntNumInput::setEditFocus(bool mark)
{
    m_spin->setEditFocus(mark);
}


// -----------------------------------------------------------------------------

QSize KIntNumInput::minimumSizeHint() const
{
    constPolish();

    int w = 0;
    int h = 0;

    // if in extra row, then count it here
    if(m_label && (m_alignment & (AlignBottom|AlignTop)))
        h += 4 + m_sizeLabel.height();
    else
        // no extra frame space
        h += m_sizeLabel.height();

    h += 2 + QMAX(m_sizeSpin.height(), m_sizeSlider.height());

    w += m_slider ? m_slider->sizeHint().width() + 8 : 0;
    w += m_colw1 + m_colw2;

    if(m_alignment & (AlignTop|AlignBottom))
        w = QMAX(w, m_sizeLabel.width() + 4);

    return QSize(w, h);
}


// -----------------------------------------------------------------------------

void KIntNumInput::doLayout()
{
    m_sizeSpin = m_spin->sizeHint();

    m_colw2 = m_sizeSpin.width();
}


// -----------------------------------------------------------------------------

void KIntNumInput::resizeEvent(QResizeEvent* e)
{
    int w = m_colw1;
    int h = 0;

    if(m_label && (m_alignment & AlignTop)) {
        m_label->setGeometry(0, 0, e->size().width(), m_sizeLabel.height());
        h += m_sizeLabel.height() + 4;
    }

    if(m_label && (m_alignment & AlignVCenter))
        m_label->setGeometry(0, 0, w, m_sizeSpin.height());

    m_spin->setGeometry(w, h, m_slider ? m_colw2 : QMAX(m_colw2, e->size().width() - w), m_sizeSpin.height());
    w += m_colw2 + 8;

    if(m_slider)
        m_slider->setGeometry(w, h, e->size().width() - w, m_sizeSpin.height());

    h += m_sizeSpin.height() + 2;

    if(m_label && (m_alignment & AlignBottom))
        m_label->setGeometry(0, h, m_sizeLabel.width(), m_sizeLabel.height());
}


// -----------------------------------------------------------------------------

KIntNumInput::~KIntNumInput()
{
}


// -----------------------------------------------------------------------------

void KIntNumInput::setValue(int val)
{
    if(m_slider)
        m_slider->setValue(val);

    m_spin->setValue(val);
}


// -----------------------------------------------------------------------------

int  KIntNumInput::value()
{
    return m_spin->value();
}


// -----------------------------------------------------------------------------

void KIntNumInput::setSpecialValueText(const QString& text)
{
    m_spin->setSpecialValueText(text);
    layout(true);
};


// -----------------------------------------------------------------------------

void KIntNumInput::setLabel(QString label, int a)
{
    KNumInput::setLabel(label, a);

    if(m_label)
        m_label->setBuddy(m_spin);
}


// -----------------------------------------------------------------------------

/**
 *
 *
 *
 *
 *
 *
 */

// -----------------------------------------------------------------------------

KDoubleNumInput::KDoubleNumInput(double value, QWidget *parent, const char *name)
    : KNumInput(parent, name)
{
    init(value);
}


// -----------------------------------------------------------------------------

KDoubleNumInput::KDoubleNumInput(KNumInput* below, double value, QWidget* parent,
                                 const char* name)
    : KNumInput(below, parent, name)
{
    init(value);
}


// -----------------------------------------------------------------------------

KDoubleNumInput::~KDoubleNumInput()
{
    delete m_format;
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::init(double value)
{
    m_value = value;
    m_format = qstrdup("%.2f");

    edit = new QLineEdit(this, "KDoubleNumInput::QLineEdit");
    edit->setAlignment(AlignRight);
    edit->setValidator(new KFloatValidator(this, "KDoubleNumInput::KFloatValidator"));

    setFocusProxy(edit);

    m_slider = 0;
    m_suffix = m_prefix = "";

    resetEditBox();
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::sliderMoved(int val)
{
    m_value = m_lower + (m_upper - m_lower)*(val/double(m_sliderstep));

    resetEditBox();
    emit valueChanged(m_value);
}


// -----------------------------------------------------------------------------

QSize KDoubleNumInput::minimumSizeHint() const
{
    constPolish();

    int w = 0;
    int h = 0;

    // if in extra row, then count it here
    if(m_label && (m_alignment & (AlignBottom|AlignTop)))
        h += 4 + m_sizeLabel.height();
    else
        // no extra frame space
        h += m_sizeLabel.height();

    h += 2 + QMAX(m_sizeEdit.height(), m_sizeSlider.height());

    w += m_slider ? m_slider->sizeHint().width() + 8 : 0;
    w += m_colw1 + m_colw2;

    if(m_alignment & (AlignTop|AlignBottom))
        w = QMAX(w, m_sizeLabel.width() + 4);

    return QSize(w, h);
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::resizeEvent(QResizeEvent* e)
{
    int w = m_colw1;
    int h = 0;

    if(m_label && (m_alignment & AlignTop)) {
        m_label->setGeometry(0, 0, e->size().width(), m_sizeLabel.height());
        h += m_sizeLabel.height() + 4;
    }

    if(m_label && (m_alignment & AlignVCenter))
        m_label->setGeometry(0, 0, w, m_sizeEdit.height());

    edit->setGeometry(w, h, m_slider ? m_colw2 : e->size().width() - w, m_sizeEdit.height());
    w += m_colw2 + 8;

    if(m_slider)
        m_slider->setGeometry(w, h, e->size().width() - w, m_sizeEdit.height());

    h += m_sizeEdit.height() + 2;

    if(m_label && (m_alignment & AlignBottom))
        m_label->setGeometry(0, h, m_sizeLabel.width(), m_sizeLabel.height());
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::doLayout()
{
    // edit sizeHint
    edit->constPolish();
    QFontMetrics fm( edit->font() );
    QString s;
    int h = fm.height();
    s.sprintf(m_format, m_value);
    int w = fm.width(m_prefix) + fm.width(s) + fm.width(m_suffix);
    w = QMAX(w, fm.width(m_specialvalue));
    if(m_range) {
        s.sprintf(m_format, m_lower);
        w = QMAX(w, fm.width(s));
        s.sprintf(m_format, m_upper);
        w = QMAX(w, fm.width(s));
        // something inbetween
        s.sprintf(m_format, m_lower + m_step);
        w = QMAX(w, fm.width(s));
    }

    if ( edit->frame() ) {
        h += 8;
        if ( edit->style() == WindowsStyle && h < 26 )
            h = 22;
        m_sizeEdit.setWidth(w + 8);
        m_sizeEdit.setHeight(h);
    } else {
        m_sizeEdit.setWidth(w + 4);
        m_sizeEdit.setHeight(h + 4);
    }

    m_colw2 = m_sizeEdit.width();

#if 0
    // label sizeHint
    m_sizeLabel = (m_label ? m_label->sizeHint() : QSize(0,0));

    if(m_label && (m_alignment & AlignVCenter))
        m_colw1 = m_sizeLabel.width();
    else
        m_colw1 = 0;

    // slider sizeHint
    m_sizeSlider = (m_slider ? m_slider->sizeHint() : QSize(0, 0));
#endif
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::setValue(double val)
{
    m_value = val;

    if(m_value < m_lower) m_value = m_lower;
    if(m_upper < m_value) m_value = m_upper;

    if(m_slider)  m_slider->setValue(val);

    resetEditBox();
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::setRange(double lower, double upper, double step, bool slider)
{
    m_lower = lower;
    m_upper = upper;
    m_step  = step;
    m_range = (m_lower < m_upper);

    // bounds checking the values
    if(m_value < m_lower)  m_value = m_lower;
    if(m_upper < m_lower)  m_upper = m_value;
    if(m_upper < m_value)  m_value = m_upper;

    // make m_value a multiple of step
    m_value = floor(m_value / m_step) * m_step;

    if(slider) {
        m_sliderstep = int(floor(QMIN((m_upper - m_lower)/m_step, double(INT_MAX-5e7))));
        m_slider = new QSlider(0, m_sliderstep, 1, floor(m_value*double(m_sliderstep)),
                               QSlider::Horizontal, this);
        m_slider->setTickmarks(QSlider::Below);
        m_slider->setTickInterval(m_slider->maxValue() / 10);
        connect(m_slider, SIGNAL(valueChanged(int)), SLOT(sliderMoved(int)));
    }
    else {
        delete m_slider;
        m_slider = 0;
    }

    resetEditBox();
    layout(true);
}


// -----------------------------------------------------------------------------

double  KDoubleNumInput::value()
{
    return m_value;
}

// -----------------------------------------------------------------------------

void KDoubleNumInput::setSuffix(QString suffix)
{
    m_suffix = suffix;

    resetEditBox();
    layout(true);
}

// -----------------------------------------------------------------------------

void KDoubleNumInput::setPrefix(QString prefix)
{
    m_prefix = prefix;

    resetEditBox();
    layout(true);
}

// -----------------------------------------------------------------------------

void KDoubleNumInput::setFormat(const char* fmt)
{
    m_format = qstrdup(fmt);

    resetEditBox();
    layout(true);
}

// -----------------------------------------------------------------------------

void KDoubleNumInput::resetEditBox()
{
    if(!m_specialvalue.isEmpty() && (fabs(m_value - m_lower) < 1e-10))
        edit->setText(m_specialvalue);
    else {
        QString s;
        s.sprintf(m_format, m_value);
        edit->setText(m_prefix + s + m_suffix);
    }
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::focusInEvent(QFocusEvent*)
{
    qDebug("focusInEvent");

    edit->setFocus();
    edit->selectAll();
}


// -----------------------------------------------------------------------------

void KDoubleNumInput::setSpecialValueText(const QString& text)
{
    m_specialvalue = text;

    resetEditBox();
    layout(true);
};


// -----------------------------------------------------------------------------

void KDoubleNumInput::setLabel(QString label, int a)
{
    KNumInput::setLabel(label, a);

    if(m_label)
        m_label->setBuddy(edit);

}

// -----------------------------------------------------------------------------

#include "knuminput.moc"
