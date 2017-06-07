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
      m_comboBox(new QComboBox),
      m_startButton(new QPushButton(tr("Start"))),
      m_stopButton(new QPushButton(tr("Stop"))),
      m_process(new QProcess(this))
{
    // Setup user interface

    // Preview
    auto previewGroupBox = new QGroupBox("Preview");
    previewGroupBox->setLayout(new QHBoxLayout);
    previewGroupBox->layout()->addWidget(m_previewLabel);
    previewGroupBox->setMinimumSize(640, 480);

    // Control
    m_startButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_stopButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_serverUrlLineEdit->setPlaceholderText("Server URL-address");
    m_rtmpNameLineEdit->setPlaceholderText("Stream name/key");

    m_comboBox = new QComboBox;
    m_comboBox->addItem(tr("Preview"));
    m_comboBox->addItem(tr("RTMP Stream"));
    m_comboBox->addItem(tr("Video Test"));
    m_comboBox->addItem(tr("RTMP Stream + Video Test"));

    auto controlLayout = new QGridLayout;
    controlLayout->addWidget(m_comboBox, 0, 0);
    controlLayout->addWidget(m_serverUrlLineEdit, 1, 0);
    controlLayout->addWidget(m_rtmpNameLineEdit, 2, 0);
    controlLayout->addWidget(m_startButton, 0, 1);
    controlLayout->addWidget(m_stopButton, 1, 1);

    m_serverUrlLineEdit->setVisible(false);
    m_rtmpNameLineEdit->setVisible(false);
    m_stopButton->setEnabled(false);

    auto controlGroupBox = new QGroupBox("Control");
    controlGroupBox->setLayout(controlLayout);
    controlGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // StreamWidget
    setLayout(new QVBoxLayout);
    layout()->addWidget(previewGroupBox);
    layout()->addWidget(controlGroupBox);

    connect(m_startButton, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMediaTypeChanged(int)));
}

void StreamWidget::closeEvent(QCloseEvent *event)
{
    stopStreaming();
    event->accept();
}

void StreamWidget::onStartButtonClicked()
{
    startStreaming();

    setStreamingStateUi();
}

void StreamWidget::onStopButtonClicked()
{
    stopStreaming();

    setPauseStateUi();
}

void StreamWidget::onProcessFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    if (exitCode != 0) {
        QByteArray messages =  m_process->readAllStandardError();
        QMessageBox::critical(this, tr("streamwidget"), messages.constData());
    }

    setPauseStateUi();
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
    QStringList arguments = makeArguments();

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

QStringList StreamWidget::makeArguments() const
{
    QString rtmpLocation = m_serverUrlLineEdit->text() + "/" + m_rtmpNameLineEdit->text();
    QStringList arguments;
    arguments << "-p";
    arguments << QString::number(m_previewLabel->winId());

    int type = m_comboBox->currentIndex();

    if (type == MT_RtmpStream || type == MT_RtmpStreamAndVideoTest) {
        arguments << "-r";
        arguments << (rtmpLocation == "/" ? QString() : rtmpLocation);
    }

    if (type == MT_RtmpStreamAndVideoTest)
        arguments << "-t";

    return arguments;
}

void StreamWidget::setPauseStateUi()
{
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_comboBox->setEnabled(true);
    m_serverUrlLineEdit->setEnabled(true);
    m_rtmpNameLineEdit->setEnabled(true);
}

void StreamWidget::setStreamingStateUi()
{
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_comboBox->setEnabled(false);
    m_serverUrlLineEdit->setEnabled(false);
    m_rtmpNameLineEdit->setEnabled(false);
}
