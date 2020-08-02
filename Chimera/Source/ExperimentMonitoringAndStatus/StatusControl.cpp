// created by Mark O. Brown
#include "stdafx.h"
#include "StatusControl.h"
#include <PrimaryWindows/IChimeraQtWindow.h>

void StatusControl::initialize (POINT& loc, IChimeraQtWindow* parent, long size,
	std::string headerText, std::vector<std::string> textColors) {
	if (textColors.size () == 0) {
		thrower ("Need to set a nonzero number of colors for status control!");
	}
	colors = textColors;
	//defaultColor = textColor;
	header = new QLabel (headerText.c_str (), parent);
	header->setFixedSize (230, 25);
	header->move (loc.x, loc.y);

	debugLevelLabel = new QLabel ("Debug Level", parent);
	debugLevelLabel->setGeometry (loc.x + 230, loc.y, 100, 25);

	debugLevelEdit = new CQLineEdit (parent);
	debugLevelEdit->setGeometry (loc.x + 330, loc.y, 50, 25);
	debugLevelEdit->setText ("-1");
	parent->connect (debugLevelEdit, &QLineEdit::textChanged, [this]() {
		try {
			currentLevel = boost::lexical_cast<unsigned>(str (debugLevelEdit->text ()));
		}
		catch (boost::bad_lexical_cast&) {
			currentLevel = 0;
		}
		addStatusText ("Changed Debug Level to \"" + str (currentLevel) + "\"\n");
		});

	clearBtn = new QPushButton (parent);
	clearBtn->setText ("Clear");
	clearBtn->setFixedSize (100, 25);
	clearBtn->move (loc.x + 380, loc.y);
	loc.y += 25;
	edit = new QPlainTextEdit (parent);
	edit->move (loc.x, loc.y);
	edit->setFixedSize (480, size);
	edit->setReadOnly (true);
	edit->setStyleSheet ("QPlainTextEdit { color: " + qstr (textColors[0]) + "; }");
	loc.y += size;
	parent->connect (clearBtn, &QPushButton::released, [this]() {clear (); });
}

void StatusControl::addStatusText (std::string text, unsigned level){
	if (currentLevel >= level) {
		if (level >= colors.size ()) {
			addStatusText (text, colors.back ());
		}
		else {
			addStatusText (text, colors[level]);
		}
	}
}

void StatusControl::addStatusText(std::string text, std::string color){
	QString htmlTxt = ("<font color = \"" + color + "\">" + text + "</font>").c_str();
	htmlTxt.replace ("\r", "");
	htmlTxt.replace ("\n", "<br/>");

	//e.g. <font color = "red">This is some text!< / font>
	//edit->appendHtml (htmlTxt);
	edit->moveCursor (QTextCursor::End);
	//edit->textCursor ().insertHtml (htmlTxt);
	//edit->insertHtml (htmlTxt);
	edit->insertPlainText (qstr(text));
	//edit->appendPlainText (qstr (text));
	edit->moveCursor (QTextCursor::End);
}

void StatusControl::clear() {
	edit->clear ();
	addStatusText("******************************\r\n", "#FFFFFF");
}


void StatusControl::appendTimebar() {
	time_t time_obj = time(0);   // get time now
	struct tm currentTime;
	localtime_s(&currentTime, &time_obj);
	std::string timeStr = "(" + str(currentTime.tm_year + 1900) + ":" + str(currentTime.tm_mon + 1) + ":"
		+ str(currentTime.tm_mday) + ") " + str(currentTime.tm_hour) + ":"
		+ str(currentTime.tm_min) + ":" + str(currentTime.tm_sec);
	addStatusText("\r\n**********" + timeStr + "**********\r\n", "#FFFFFF");
}

