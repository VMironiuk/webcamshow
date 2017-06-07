#include "streamwidget.h"

#include <QtWidgets>

enum MediaType
{
    MT_Preview,
    MT_RtmpStream,
    MT_VideoTest,
    MT_RtmpStreamAndVideoTest
};

StreamWidget::StreamWidget(QWidget *parent)
    : QWidget(parent),
      m_previewLabel(new QLabel),
      m_serverUrlLineEdit(new QLineEdit),
      m_rtmpNameLineEdit(new QLineEdit),
      m_process(new QProcess(this))
{
    // Setup user interface

    // Preview
    auto previewGroupBox = new QGroupBox("Preview");
    previewGroupBox->setLayout(new QHBoxLayout);
    previewGroupBox->layout()->addWidget(m_previewLabel);
    previewGroupBox->setMinimumSize(640, 480);

    // Control
    auto startButton = new QPushButton("Start");
    startButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto stopButton = new QPushButton("Stop");
    stopButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_serverUrlLineEdit->setPlaceholderText("Server URL-address");
    m_rtmpNameLineEdit->setPlaceholderText("Stream name/key");

    auto comboBox = new QComboBox;
    comboBox->addItem(tr("Preview"));
    comboBox->addItem(tr("RTMP Stream"));
    comboBox->addItem(tr("Video Test"));
    comboBox->addItem(tr("RTMP Stream + Video Test"));

    auto controlLayout = new QGridLayout;
    controlLayout->addWidget(comboBox, 0, 0);
    controlLayout->addWidget(m_serverUrlLineEdit, 1, 0);
    controlLayout->addWidget(m_rtmpNameLineEdit, 2, 0);
    controlLayout->addWidget(startButton, 0, 1);
    controlLayout->addWidget(stopButton, 1, 1);

    m_serverUrlLineEdit->setVisible(false);
    m_rtmpNameLineEdit->setVisible(false);

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

    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMediaTypeChanged(int)));
}

void StreamWidget::closeEvent(QCloseEvent *event)
{
    stopStreaming();
    event->accept();
}

void StreamWidget::onStartButtonClicked()
{
    startStreaming();
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

void StreamWidget::onMediaTypeChanged(int type)
{
    switch (type) {
    case MT_Preview: case MT_VideoTest:
        m_serverUrlLineEdit->setVisible(false);
        m_rtmpNameLineEdit->setVisible(false);
        break;

    case MT_RtmpStream: case MT_RtmpStreamAndVideoTest:
        m_serverUrlLineEdit->setVisible(true);
        m_rtmpNameLineEdit->setVisible(true);
        break;

    default:
        break;
    }
}

void StreamWidget::startStreaming() const
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

void StreamWidget::stopStreaming() const
{
    if (!m_process->isOpen())
        return;

    m_process->write("q"); // notify streamer to quit

    if (!m_process->waitForFinished(3000)) {
        m_process->terminate();
    }
}
