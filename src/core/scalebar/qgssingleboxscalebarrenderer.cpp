/***************************************************************************
                            qgssingleboxscalebarrenderer.cpp
                            --------------------------------
    begin                : June 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco.hugentobler@karto.baug.ethz.ch
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgssingleboxscalebarrenderer.h"
#include "qgsscalebarsettings.h"
#include "qgslayoututils.h"
#include <QList>
#include <QPainter>

QString QgsSingleBoxScaleBarRenderer::id() const
{
  return QStringLiteral( "Single Box" );
}

QString QgsSingleBoxScaleBarRenderer::visibleName() const
{
  return QObject::tr( "Single Box" );
}

int QgsSingleBoxScaleBarRenderer::sortKey() const
{
  return 1;
}

QgsSingleBoxScaleBarRenderer *QgsSingleBoxScaleBarRenderer::clone() const
{
  return new QgsSingleBoxScaleBarRenderer( *this );
}

void QgsSingleBoxScaleBarRenderer::draw( QgsRenderContext &context, const QgsScaleBarSettings &settings, const ScaleBarContext &scaleContext ) const
{
  if ( !context.painter() )
  {
    return;
  }
  QPainter *painter = context.painter();

  double scaledLabelBarSpace = context.convertToPainterUnits( settings.labelBarSpace(), QgsUnitTypes::RenderMillimeters );
  double scaledBoxContentSpace = context.convertToPainterUnits( settings.boxContentSpace(), QgsUnitTypes::RenderMillimeters );
  QFontMetricsF fontMetrics = QgsTextRenderer::fontMetrics( context, settings.textFormat() );
  double barTopPosition = scaledBoxContentSpace + ( settings.labelVerticalPlacement() == QgsScaleBarSettings::LabelAboveSegment ? fontMetrics.ascent() + scaledLabelBarSpace : 0 );

  painter->save();
  if ( context.flags() & QgsRenderContext::Antialiasing )
    painter->setRenderHint( QPainter::Antialiasing, true );

  QPen pen = settings.pen();
  pen.setWidthF( context.convertToPainterUnits( pen.widthF(), QgsUnitTypes::RenderMillimeters ) );
  painter->setPen( pen );

  bool useColor = true; //alternate brush color/white
  double xOffset = firstLabelXOffset( settings, context, scaleContext );

  QList<double> positions = segmentPositions( scaleContext, settings );
  QList<double> widths = segmentWidths( scaleContext, settings );

  for ( int i = 0; i < positions.size(); ++i )
  {
    if ( useColor ) //alternating colors
    {
      painter->setBrush( settings.brush() );
    }
    else //secondary color
    {
      painter->setBrush( settings.brush2() );
    }

    QRectF segmentRect( context.convertToPainterUnits( positions.at( i ), QgsUnitTypes::RenderMillimeters ) + xOffset,
                        barTopPosition, context.convertToPainterUnits( widths.at( i ), QgsUnitTypes::RenderMillimeters ),
                        context.convertToPainterUnits( settings.height(), QgsUnitTypes::RenderMillimeters ) );
    painter->drawRect( segmentRect );
    useColor = !useColor;
  }

  painter->restore();

  //draw labels using the default method
  drawDefaultLabels( context, settings, scaleContext );
}



