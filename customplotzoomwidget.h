
#pragma once

#include <QPoint>
#include "../3rd_party/qcustomplot/qcustomplot.h"

class QRubberBand;
class QMouseEvent;
class QWidget;

class CustomPlotZoomWidget : public QCustomPlot
{
    Q_OBJECT

public:
    CustomPlotZoomWidget(QWidget * parent = 0);
    virtual ~CustomPlotZoomWidget();

    void setZoomMode(bool mode);

private slots:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

    //Context menu
    void ShowContextMenu(const QPoint &pos);
    void OpenConfigPlot();

private:
    bool mZoomMode;
    QRubberBand * mRubberBand;
    QPoint mOrigin;
};
