#include <QApplication>
#include <QMainWindow>
#include "EToastr.h"
#include <qdebug.h>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	

    EWidget* mainWin = new EWidget(); mainWin->setWindowTitle("EToastr | github.com/emrecpp");
    if (1)
        mainWin->resize(640, 360);
    else
        mainWin->resize(1280, 720);

    bool test_wait_gui_load = 1;

	auto f = [mainWin](){
        for (int i = 0; i < 3; i++) {

			// attr
            const bool PARENT_IS_DESKTOP = false;
            const bool NO_TIMEOUT = false;
            const bool SHOW_ICON = true;
            


			EToastr* toastr = new EToastr(((!PARENT_IS_DESKTOP) ? mainWin : nullptr), SHOW_ICON);
			toastr->setStyle(EToastr::TOASTR_STYLE::INFO);

            if (1)
                toastr->setText("Hello! " + QString::number(i + 1));
            else
                toastr->setText(R"(Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.)" 
                    + QString::number(i + 1));

            toastr->setDuration((!NO_TIMEOUT) ? (1000 * (i + 1)) : 0);
            if (1)
				toastr->show(EToastr::TOASTR_DIRECTION::RIGHT_TO_LEFT);
            else if (1)
                toastr->show(EToastr::TOASTR_DIRECTION::LEFT_TO_RIGHT);
            else if (0)
				toastr->show(EToastr::TOASTR_DIRECTION::BOTTOM_TO_TOP);
            else if (1)
				toastr->show(EToastr::TOASTR_DIRECTION::TOP_TO_BOTTOM);
            
		}
    };

    if (test_wait_gui_load){ // waits GUI loading.
        QTimer::singleShot(500, [f]() {f();});
    }
	else
		f();
	mainWin->show();
	return app.exec();
}
