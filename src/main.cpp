#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    // 1. Qt 애플리케이션 객체 생성
    QApplication app(argc, argv);

    // 2. 우리가 만든 메인 윈도우 생성
    MainWindow window;

    // 3. 윈도우 제목 설정
    window.setWindowTitle("DCDC Power Stage AC Analyzer v0.1b");

    // 4. 화면에 표시
    window.show();

    // 5. 이벤트 루프 시작 (창이 닫힐 때까지 프로그램 유지)
    return app.exec();
}