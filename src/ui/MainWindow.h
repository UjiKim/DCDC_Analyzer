#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis> // [수정] SI 단위 표기를 위해 추가
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>

#include "models/PowerStageParams.h"
#include "models/AnalysisResult.h"
#include "core/ConverterBase.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCalculateClicked(); // [핵심] 계산 및 로직 동기화 슬롯
    void onExportClicked(); // [추가] CSV 내보내기 슬롯
private:
    void setupUi();
    void setupCharts();
    void updatePlots(const AnalysisResult& result);

    void initChart(QChartView* view, QLineSeries* series, QCategoryAxis*& axisX, QValueAxis*& axisY,
        const QString& title, const QString& yTitle, double yMin, double yMax, int yTickCount, const QColor& color);

    //예외처리
    bool showValidationError(const QString& message);   // 입력 오류 표시용
    bool showCriticalError(const QString& message);   // 치명적 설계 오류용
    bool showWarning(const QString& message);   //경고표시용

    void setupInputValidators();
    bool validateInputs();  // 입력값 유효성 검사

    //토폴로지별 검증
    bool validateBuckConstraints(double vin, double vout);
    bool validateBoostConstraints(double vin, double vout);

    // UI Controls
    QLineEdit *m_editVin, *m_editVout, *m_editL, *m_editC, *m_editIout, *m_editFsw, *m_editRdcr, *m_editResr, *m_editRon;
    QRadioButton *m_rbBuck, *m_rbBoost, *m_rbGd, *m_rbGv;
    QPushButton *m_btnExport;
    // Charts
    QChartView *m_chartViewMag, *m_chartViewPhase;
    QLineSeries *m_seriesMag, *m_seriesPhase;
    QCategoryAxis *m_axisXMag, *m_axisXPhase; // [수정] CategoryAxis 사용
    QValueAxis *m_axisYMag, *m_axisYPhase;

    // Results
    QLabel *m_lblMode, *m_lblD1, *m_lblD2, *m_lblM, *m_lblDcGain, *m_lblWp, *m_lblFc, *m_lblPm;
    AnalysisResult m_lastResult;    // 최신 계산 결과 저장용
};
#endif