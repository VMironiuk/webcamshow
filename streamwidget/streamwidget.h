#ifndef STREAMWIDGET_H
#define STREAMWIDGET_H

#include <QWidget>
#include <QProcess>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class StreamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StreamWidget(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void onStartButtonClicked();
    void onStopButtonClicked();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onMediaTypeChanged(int type);

private:
    void startStreaming() const;
    void stopStreaming() const;
    QStringList makeArguments() const;
    void setPauseStateUi();
    void setStreamingStateUi();

    QLabel *m_previewLabel;
    QLineEdit *m_serverUrlLineEdit;
    QLineEdit *m_rtmpNameLineEdit;
    QComboBox *m_comboBox;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QProcess *m_process;
};

#endif // STREAMWIDGET_H
