#include "ui/MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QtCharts/QCategoryAxis> // [수정] QLogValueAxis 대신 QCategoryAxis 사용
#include <QtCharts/QValueAxis>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QToolTip>
#include <QCursor>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QPixmap>
#include <QIcon>

#include "utils/MathUtils.h"
#include <cmath>

#include "core/BuckConverter.h"  // BuckConverter 정의 포함
#include "core/BoostConverter.h" // BoostConverter 정의 포함

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    setupCharts();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    // [추가] 윈도우 창 아이콘 설정
    setWindowIcon(QIcon("lablogo.png"));
    
    auto *centralWidget = new QWidget(this);
    // [수정] 전체를 감싸는 루트 레이아웃을 수직(QVBoxLayout)으로 구성하여 하단 영역을 확보합니다.
    auto *rootLayout = new QVBoxLayout(centralWidget);

    // 기존의 상단 작업 영역(입력 패널 + 그래프)을 위한 레이아웃입니다.
    auto *contentLayout = new QHBoxLayout();

    // --- [좌측: 입력 패널] ---
    auto *leftPanel = new QVBoxLayout();
    
    // 1. 설계 파라미터 그룹
    auto *inputGroup = new QGroupBox("Design Parameters");
    auto *formLayout = new QFormLayout(inputGroup);

    m_editVin = new QLineEdit("12.0");
    m_editVout = new QLineEdit("5.0");
    m_editL = new QLineEdit("10e-6");
    m_editC = new QLineEdit("22e-6");
    m_editIout = new QLineEdit("2.0");
    m_editFsw = new QLineEdit("1e6");
    m_editRdcr = new QLineEdit("150e-3");
    m_editResr = new QLineEdit("150e-3");
    m_editRon = new QLineEdit("500e-3");

    formLayout->addRow("Input Voltage [V]:", m_editVin);
    formLayout->addRow("Output Voltage [V]:", m_editVout);
    formLayout->addRow("Inductance [H]:", m_editL);
    formLayout->addRow("Capacitance [F]:", m_editC);
    formLayout->addRow("Load Current [A]:", m_editIout);
    formLayout->addRow("Switching Freq. [Hz]:", m_editFsw);
    formLayout->addRow("Inductor DCR [Ohm]:", m_editRdcr);
    formLayout->addRow("Capacitor ESR [Ohm]:", m_editResr);
    formLayout->addRow("On-Resistance [Ohm]:", m_editRon);
    leftPanel->addWidget(inputGroup);

    // 2. 실행 버튼 영역
    auto *btnCalc = new QPushButton("Calculate & Plot");
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    
    m_btnExport = new QPushButton("Export to CSV");
    m_btnExport->setEnabled(false); 
    connect(m_btnExport, &QPushButton::clicked, this, &MainWindow::onExportClicked);

    leftPanel->addWidget(btnCalc);
    leftPanel->addWidget(m_btnExport);

    // 3. 토폴로지 및 해석 타입 선택
    auto *topoGroup = new QGroupBox("Topology");
    auto *topoLayout = new QVBoxLayout(topoGroup);
    m_rbBuck = new QRadioButton("Buck");
    m_rbBoost = new QRadioButton("Boost");
    m_rbBuck->setChecked(true);
    topoLayout->addWidget(m_rbBuck);
    topoLayout->addWidget(m_rbBoost);
    leftPanel->addWidget(topoGroup);

    auto *typeGroup = new QGroupBox("Analysis Type");
    auto *typeLayout = new QVBoxLayout(typeGroup);
    m_rbGd = new QRadioButton("Control-to-Output (Gd)");
    m_rbGv = new QRadioButton("Input-to-Output (Gv)");
    m_rbGd->setChecked(true);
    typeLayout->addWidget(m_rbGd);
    typeLayout->addWidget(m_rbGv);
    leftPanel->addWidget(typeGroup);

    // 4. 결과 표시 그룹
    auto *resultGroup = new QGroupBox("Calculated Parameters");
    auto *resultLayout = new QFormLayout(resultGroup);
    m_lblMode = new QLabel("-");
    m_lblD1 = new QLabel("-");
    m_lblD2 = new QLabel("-");
    m_lblM = new QLabel("-");
    m_lblDcGain = new QLabel("-");
    m_lblWp = new QLabel("-");
    m_lblFc = new QLabel("-");
    m_lblPm = new QLabel("-");

    resultLayout->addRow("Operating Mode:", m_lblMode);
    resultLayout->addRow("Duty Cycle (D1):", m_lblD1);
    resultLayout->addRow("DCM Duty (D2):", m_lblD2);
    resultLayout->addRow("Conversion Ratio (M):", m_lblM);
    resultLayout->addRow("DC Gain (G0):", m_lblDcGain);
    resultLayout->addRow("Dominant Pole Frequency (f0/fp):", m_lblWp);
    resultLayout->addRow("Crossover Frequency (fc):", m_lblFc);
    resultLayout->addRow("Phase Margin (PM):", m_lblPm);
    leftPanel->addWidget(resultGroup);

    leftPanel->addStretch(); 

    // --- [우측: 그래프 패널] ---
    auto *rightPanel = new QVBoxLayout();
    m_chartViewMag = new QChartView();
    m_chartViewPhase = new QChartView();
    rightPanel->addWidget(m_chartViewMag);
    rightPanel->addWidget(m_chartViewPhase);

    // 상단 컨텐츠 레이아웃 병합
    contentLayout->addLayout(leftPanel, 1);
    contentLayout->addLayout(rightPanel, 3);
    rootLayout->addLayout(contentLayout, 1);

    // --- [하단: 저작권 및 연구실 메시지 영역] ---
    auto *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    rootLayout->addWidget(line);

    // [수정] 로고(좌)와 텍스트(우) 배치를 위한 하단 전용 레이아웃
    auto *footerLayout = new QHBoxLayout();

    // 1. 좌측 하단 연구실 로고 (lablogo.png)
    auto *lblLogo = new QLabel();
    QPixmap logoPixmap("lablogo.png");
    if (!logoPixmap.isNull()) {
        lblLogo->setPixmap(logoPixmap.scaled(200, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblLogo->setText("[AIMLAB Logo]");
        lblLogo->setStyleSheet("color: #bdc3c7; font-weight: bold; padding-left: 10px;");
    }
    footerLayout->addWidget(lblLogo);

    // 2. 중간 유연한 여백 (로고와 텍스트를 양 끝으로 밀어냄)
    footerLayout->addStretch();

    // 3. 우측 하단 멘트 및 하이퍼링크
    auto *lblFooter = new QLabel(
        "본 프로그램은 <a href='https://sites.google.com/view/amp-lab/' style='color: #7f8c8d; text-decoration: underline;'><b>AIMLAB(전현탁 교수님 연구실)</b></a>의 연구목적으로 제작되었으며, "
        "상업적 이용 및 무단복제를 금지합니다.<br>"
        "Copyright © 2026 wjkim. All rights reserved."
    );
    lblFooter->setOpenExternalLinks(true);
    lblFooter->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lblFooter->setStyleSheet(
        "QLabel {"
        "  color: #7f8c8d;"
        "  font-size: 10pt;"
        "  padding: 10px;"
        "  line-height: 150%;"
        "}"
    );
    footerLayout->addWidget(lblFooter);

    rootLayout->addLayout(footerLayout);

    setCentralWidget(centralWidget);
    resize(1200, 950); // 로고와 푸터 높이를 고려하여 창 높이 소폭 조정
}

void MainWindow::setupCharts() {
    // 1. Magnitude 차트 초기화
    m_seriesMag = new QLineSeries();
    initChart(m_chartViewMag, m_seriesMag, m_axisXMag, m_axisYMag, 
              "Magnitude Response (dB)", "Magnitude [dB]", -60.0, 40.0, 6, Qt::red);

    // 2. Phase 차트 초기화
    m_seriesPhase = new QLineSeries();
    initChart(m_chartViewPhase, m_seriesPhase, m_axisXPhase, m_axisYPhase, 
              "Phase Response (Degree)", "Phase [Degree]", -225.0, 90.0, 8, Qt::blue);
}

// [신규 함수] 공통 초기화 로직
void MainWindow::initChart(QChartView* view, QLineSeries* series, QCategoryAxis*& axisX, QValueAxis*& axisY, 
                           const QString& title, const QString& yTitle, double yMin, double yMax, int yTickCount, const QColor& color) {
    auto *chart = new QChart();
    chart->addSeries(series);

    // [추가] 선 색상 및 두께 설정
    QPen pen(color);
    pen.setWidth(2); // 선 두께를 2px로 설정하여 가독성 향상
    series->setPen(pen);

    // 차트제목설정
    chart->setTitle(title);
    QFont titleFont = chart->titleFont();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    chart->setTitleFont(titleFont);
    chart->legend()->hide(); // 범례 제거

    // [추가] 툴팁 기능 구현: 데이터 포인트 마우스 호버링 감지
    series->setPointsVisible(false); // 데이터 포인트 지점을 명확히 표시 : true
    connect(series, &QLineSeries::hovered, [this, view, yTitle](const QPointF &point, bool state) {
        if (state) {
            // X축 좌표(log10 주파수)를 실제 주파수로 역계산
            double freq = std::pow(10.0, point.x());
            QString freqStr = MathUtils::formatSi(freq, "Hz");
            
            // Y축 값 및 단위 결정
            QString valStr = QString::number(point.y(), 'f', 2);
            QString unit = yTitle.contains("dB") ? "dB" : "deg";
            
            QString tooltipText = QString("주파수: %1\n값: %2 %3")
                                    .arg(freqStr)
                                    .arg(valStr)
                                    .arg(unit);
            
            // 현재 마우스 위치에 툴팁 표시
            QToolTip::showText(QCursor::pos(), tooltipText, view);
        } else {
            // 마우스가 포인터를 벗어나면 툴팁 숨김
            QToolTip::hideText();
        }
    });

    // X축 설정 (Log-Linear 매핑용 SI 단위 축)
    axisX = new QCategoryAxis();
    axisX->setTitleText("Frequency [Hz]");
    axisX->append("100m", -1); 
    axisX->append("1", 0);   axisX->append("10", 1);  axisX->append("100", 2);
    axisX->append("1k", 3);  axisX->append("10k", 4); axisX->append("100k", 5);
    axisX->append("1M", 6);  axisX->append("10M", 7); axisX->append("100M", 8);
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    // Y축 설정
    axisY = new QValueAxis();
    axisY->setTitleText(yTitle);
    axisY->setRange(yMin, yMax);
    axisY->setTickCount(yTickCount);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    view->setChart(chart);
    view->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::onCalculateClicked() {

    if (!validateInputs()) {
        return; // 검증 실패 시 계산을 진행하지 않고 중단
    }

    PowerStageParams params;
    // 1. 입력 설정
    params.setVin(m_editVin->text().toDouble());
    params.setVout(m_editVout->text().toDouble());
    params.setTopology(m_rbBuck->isChecked() ? Topology::Buck : Topology::Boost);
    params.setL(m_editL->text().toDouble());
    params.setC(m_editC->text().toDouble());
    params.setIout(m_editIout->text().toDouble());
    params.setFsw(m_editFsw->text().toDouble());

    // 2. 내부 상태 동기화 (Duty, M, K 일괄 계산)
    params.updateInternalState();

    // 3. 컨버터 및 해석 타입 결정
    std::unique_ptr<ConverterBase> converter;
    if (params.topology() == Topology::Buck) {
        converter = std::make_unique<BuckConverter>(params);
    } else {
        converter = std::make_unique<BoostConverter>(params);
    }

    // 4. 주파수 스윕 및 데이터 수집
    auto sweepFreqs = MathUtils::generateLogSpace(1e-1, 1e8, 100); // 변수명 변경 (freqs -> sweepFreqs)
    AnalysisResult result;
    for (double f : sweepFreqs) {
        std::complex<double> resp = m_rbGd->isChecked() ? converter->getGd(f) : converter->getGv(f);
        result.addPoint(f, MathUtils::toDb(resp), MathUtils::toPhase(resp));
    }

    // 4-1. 익스포트용 멤버 변수로 저장
    m_lastResult = result;
    m_btnExport->setEnabled(true); // 계산 완료 후 버튼 활성화

    // 5. 결과 분석 (DC Gain, fp, fc, PM)
    // 5-1. DC Gain 계산
    std::complex<double> dcResp = m_rbGd->isChecked() ? converter->getGd(1e-3) : converter->getGv(1e-3);
    double dcGainDb = MathUtils::toDb(dcResp);

    // 5-2. Primary Pole (fp) 계산
    double fp = 0.0;
    const double R = params.Rload();
    const double L = params.L();
    const double C = params.C();
    const double M = params.M();

    if (params.topology() == Topology::Buck) {
        if (params.mode() == OperationMode::CCM) {
            fp = 1.0 / (2.0 * std::numbers::pi * std::sqrt(L * C)); //
        } else {
            fp = ((2.0 - M) / (1.0 - M)) * (1.0 / (2.0 * std::numbers::pi * R * C)); //
        }
    } else { // Boost
        if (params.mode() == OperationMode::CCM) {
            fp = (1.0 - params.d1()) / (2.0 * std::numbers::pi * std::sqrt(L * C)); //
        } else {
            fp = ((2.0 * M - 1.0) / (M - 1.0)) * (1.0 / (2.0 * std::numbers::pi * R * C)); //
        }
    }

    // 5-3. Crossover Frequency (fc) 및 Phase Margin (PM) 계산
    double fc = 0.0;
    double pm = 0.0;
    bool fcFound = false;
    const auto& resFreqs = result.frequencies(); // 중복 선언 방지를 위해 이름 변경
    const auto& resMags = result.magnitudes();
    const auto& resPhases = result.phases();

    for (size_t i = 0; i < result.count() - 1; ++i) {
        if (resMags[i] >= 0.0 && resMags[i + 1] < 0.0) {
            double ratio = resMags[i] / (resMags[i] - resMags[i + 1]);
            fc = resFreqs[i] + ratio * (resFreqs[i + 1] - resFreqs[i]);
            double phaseAtFc = resPhases[i] + ratio * (resPhases[i + 1] - resPhases[i]);
            pm = 180.0 + phaseAtFc; 
            fcFound = true;
            break;
        }
    }

    // 6. UI 결과 업데이트
    m_lblMode->setText(params.mode() == OperationMode::CCM ? "CCM" : "DCM");
    m_lblMode->setStyleSheet(params.mode() == OperationMode::CCM ? "color: blue; font-weight: bold;" : "color: red; font-weight: bold;");
    m_lblD1->setText(QString::number(params.d1(), 'f', 6));
    m_lblD2->setText(params.mode() == OperationMode::DCM ? QString::number(params.d2(), 'f', 6) : "N/A");
    m_lblM->setText(QString::number(params.M(), 'f', 3));
    m_lblDcGain->setText(MathUtils::formatSi(dcGainDb, "dB"));
    m_lblWp->setText(MathUtils::formatSi(fp, "Hz"));

    if (fcFound) {
        m_lblFc->setText(MathUtils::formatSi(fc, "Hz"));
        m_lblPm->setText(QString::number(pm, 'f', 3) + " deg");
    } else {
        m_lblFc->setText("N/A");
        m_lblPm->setText("N/A");
    }

    updatePlots(result);
}

void MainWindow::onExportClicked() {
    if (m_lastResult.count() == 0) return;

    // 1. 파일 저장 경로 선택
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Analysis Result", "", "CSV Files (*.csv);;All Files (*)");

    if (fileName.isEmpty()) return;

    // 2. 파일 쓰기 로직
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        
        // CSV 헤더 작성
        QString analysisType = m_rbGd->isChecked() ? "Gd (Control-to-Output)" : "Gv (Input-to-Output)";
        out << "Analysis Type: " << analysisType << "\n";
        out << "Frequency [Hz],Magnitude [dB],Phase [deg]\n";

        // 데이터 기록
        const auto& freqs = m_lastResult.frequencies();
        const auto& mags = m_lastResult.magnitudes();
        const auto& phases = m_lastResult.phases();

        for (size_t i = 0; i < m_lastResult.count(); ++i) {
            out << QString::number(freqs[i], 'f', 4) << ","
                << QString::number(mags[i], 'f', 4) << ","
                << QString::number(phases[i], 'f', 4) << "\n";
        }

        file.close();
        QMessageBox::information(this, "Export 성공", "데이터가 성공적으로 저장되었습니다.");
    } else {
        showCriticalError("파일을 열 수 없습니다.");
    }
}

void MainWindow::updatePlots(const AnalysisResult& result) {
m_seriesMag->clear();
    m_seriesPhase->clear();

    double maxMag = -1e9;
    double minMag = 1e9;

    for (size_t i = 0; i < result.count(); ++i) {
        double f = result.frequencies()[i];
        double mag = result.magnitudes()[i];
        double phase = result.phases()[i];
        
        double logFreq = std::log10(f);
        m_seriesMag->append(logFreq, mag);
        m_seriesPhase->append(logFreq, phase);

        // 최댓값, 최솟값 추적
        if (mag > maxMag) maxMag = mag;
        if (mag < minMag) minMag = mag;
    }

    // [수정] X축 표시 범위 설정 (log10(0.1) = -1, log10(100M) = 8)
    m_axisXMag->setRange(-1, 8);
    m_axisXPhase->setRange(-1, 8);

    // [추가] Magnitude Y축 자동 스케일링 (20dB 간격)
    double yTop = std::ceil(maxMag / 20.0) * 20.0;
    double yBottom = std::floor(minMag / 20.0) * 20.0;
    
    // 데이터가 평탄할 경우를 대비한 최소 범위 확보
    if (yTop <= yBottom) { yTop += 20; yBottom -= 20; }

    m_axisYMag->setRange(yBottom, yTop);
    // 20dB 간격으로 Tick 개수 계산
    int tickCount = static_cast<int>((yTop - yBottom) / 20) + 1;
    m_axisYMag->setTickCount(tickCount);
}

// 중복 코드를 줄이기 위한 헬퍼 함수
bool MainWindow::showValidationError(const QString& message) {
    QMessageBox::warning(this, "입력 오류", message);
    return false;
}

bool MainWindow::showCriticalError(const QString& message) {
    QMessageBox::critical(this, "설계 오류", message);
    return false;
}

bool MainWindow::showWarning(const QString& message) {
    QMessageBox::warning(this, "설계 경고", message);
    return true;
}

void MainWindow::setupInputValidators() {
    // 양수만 허용하고 과학적 표기법을 지원하는 검사기
    QDoubleValidator *validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::ScientificNotation);
    validator->setBottom(0.0); // 0보다 큰 값만 허용 (필요시)

    // 모든 입력 필드에 적용
    m_editVin->setValidator(validator);
    m_editVout->setValidator(validator);
    m_editL->setValidator(validator);
    m_editC->setValidator(validator);
    m_editIout->setValidator(validator);
    m_editFsw->setValidator(validator);
    m_editRdcr->setValidator(validator);
    m_editResr->setValidator(validator);
    m_editRon->setValidator(validator);
}

bool MainWindow::validateInputs() {
// 검사할 필드들과 그에 해당하는 이름을 매핑 (반복문으로 처리하면 깔끔합니다)
    struct InputField {
        QLineEdit* edit;
        QString name;
    };

    QList<InputField> fields = {
        {m_editVin, "입력 전압(Vin)"},
        {m_editVout, "출력 전압(Vout)"},
        {m_editL, "인덕턴스(L)"},
        {m_editC, "커패시턴스(C)"},
        {m_editIout, "출력 전류(Iout)"},
        {m_editFsw, "스위칭 주파수(fsw)"},
        {m_editRdcr, "인덕턴스 직렬 저항(DCR)"},
        {m_editResr, "커패시터 직렬 저항(ESR)"},
        {m_editRon, "스위치 on 저항(Ron)"}
    };

    for (const auto& field : fields) {
        QString text = field.edit->text().trimmed(); // 앞뒤 공백 제거

        // 1. 비어있는지 체크
        if (text.isEmpty()) {
            return showValidationError(field.name + " 값이 비어있습니다.");
        }

        // 2. 유효한 숫자인지 체크 (과학적 표기법 포함)
        bool ok;
        double value = text.toDouble(&ok);
        if (!ok) {
            return showValidationError(field.name + "에 유효하지 않은 숫자 형식인 '" + text + "'이 입력되었습니다.");
        }

        // 3. 물리적인 양수값 체크 (Iout은 0 가능, 나머지는 양수여야 함)
        if (field.name == "출력 전류(Iout)") {
            if (value < 0) return showValidationError(field.name + "은 0 이상의 값이어야 합니다.");
        } else {
            if (value <= 0) return showValidationError(field.name + "은 0보다 큰 값이어야 합니다.");
        }
    }

    // 4. 기존 토폴로지 제약 조건 체크 (Critical Error)
    double vin = m_editVin->text().toDouble();
    double vout = m_editVout->text().toDouble();

    if (m_rbBuck->isChecked() && vout >= vin) {
        if (!validateBuckConstraints(vin, vout)) return false;
    }
    
    if (m_rbBoost->isChecked() && vout <= vin) {
        if (!validateBoostConstraints(vin, vout)) return false;
    }

    return true;
}

bool MainWindow::validateBuckConstraints(double vin, double vout) {
    double d_ideal = vout / vin;
    // Buck: Vout < Vin 필수
    if (vout >= vin) {
        return showCriticalError(
            QString("Buck 컨버터 제약 조건 위반: 출력이 입력보다 낮아야 합니다.\n(입력: %1V, 출력: %2V)")
            .arg(vin).arg(vout)
        );
    }
    
    // Duty 90초과
    if (d_ideal > 0.90) {
        return showWarning(QString("Buck Duty Cycle이 매우 높습니다 (D_ideal: %1).\n"
                    "High Duty 동작 시 전압 드랍으로 인해 목표 전압 유지가 어려울 수 있습니다.")
            .arg(d_ideal, 0, 'f', 4));
    }

    // Duty < 0.05
    if (d_ideal <= 0.05) {
        return showWarning(
            QString("Buck Duty Cycle이 너무 낮습니다 (D_ideal: %1).\n"
                    "최소 On-time 제약으로 인해 제어가 불안정할 수 있습니다.")
            .arg(d_ideal, 0, 'f', 4)
        );
    }
    return true;
}

bool MainWindow::validateBoostConstraints(double vin, double vout) {
    double d_ideal = (vout - vin) / vout;
    // Boost: Vout > Vin 필수
    if (vout <= vin) {
        return showCriticalError(
            QString("Boost 컨버터 제약 조건 위반: 출력이 입력보다 높아야 합니다.\n(입력: %1V, 출력: %2V)")
            .arg(vin).arg(vout)
        );
    }
    
    // Duty 0.05 이하
    if (d_ideal <= 0.05) {
        return showWarning(
            QString("Boost Duty Cycle이 너무 낮습니다 (D_ideal: %1).\n"
                    "스위칭 노이즈 및 제어 해상도 문제가 발생할 수 있습니다.")
            .arg(d_ideal, 0, 'f', 4)
        );
    }
    
    // 2. 설계 권고 Duty 초과
    if (d_ideal > 0.90) {
        return showWarning(QString("Boost Duty Cycle이 매우 높습니다 (D_ideal: %1).\n"
                    "효율이 급감하고 스위치 소자의 스트레스가 심해질 수 있습니다.")
            .arg(d_ideal, 0, 'f', 4));
    }

    return true;
}