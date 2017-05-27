#include "streamwidget.h"

#include <QtWidgets>
#include <QDebug>

StreamWidget::StreamWidget(QWidget *parent)
    : QWidget(parent),
      m_previewLabel(new QLabel),
      m_serverUrlLineEdit(new QLineEdit),
      m_rtmpNameLineEdit(new QLineEdit),
      m_process(new QProcess(this))
{
    // Set up user interface

    // Preview
    auto previewGroupBox = new QGroupBox("Preview");
    previewGroupBox->setLayout(new QHBoxLayout);
    previewGroupBox->layout()->addWidget(m_previewLabel);
    previewGroupBox->setMinimumSize(640, 480);

    // Control
    auto startButton = new QPushButton("Start");
    auto stopButton = new QPushButton("Stop");

    m_serverUrlLineEdit->setPlaceholderText("Server URL-address");
    m_rtmpNameLineEdit->setPlaceholderText("Stream name/key");

    auto controlLayout = new QGridLayout;
    controlLayout->addWidget(m_serverUrlLineEdit, 0, 0);
    controlLayout->addWidget(m_rtmpNameLineEdit, 1, 0);
    controlLayout->addWidget(startButton, 0, 1);
    controlLayout->addWidget(stopButton, 1, 1);

    auto controlGroupBox = new QGroupBox("Control");
    controlGroupBox->setLayout(controlLayout);
    controlGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // StreamWidget
    setLayout(new QVBoxLayout);
    layout()->addWidget(previewGroupBox);
    layout()->addWidget(controlGroupBox);

    connect(startButton, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));
}

void StreamWidget::closeEvent(QCloseEvent *event)
{
    stopStreaming();
    event->accept();
}

void StreamWidget::onStartButtonClicked()
{
    QString program = "./streamer";
    QString rtmpLocation = m_serverUrlLineEdit->text() + "/" + m_rtmpNameLineEdit->text();
    QStringList arguments;
    arguments << "-p";
    arguments << QString::number(m_previewLabel->winId());
    arguments << "-r";
    arguments << (rtmpLocation == "/" ? QString() : rtmpLocation);

    m_process->start(program, arguments);
}

void StreamWidget::onStopButtonClicked()
{
    stopStreaming();
}

void StreamWidget::onProcessFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    if (exitCode != 0) {
        QByteArray messages =  m_process->readAllStandardError();
        QMessageBox::critical(this, tr("streamwidget"), messages.constData());
    }
}

void StreamWidget::stopStreaming() const
{
    if (!m_process->isOpen())
        return;

    m_process->write("q"); // notify streamer to quit

    if (!m_process->waitForFinished(3000)) {
        m_process->terminate();
    }
}
