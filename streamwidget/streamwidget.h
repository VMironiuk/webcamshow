#ifndef STREAMWIDGET_H
#define STREAMWIDGET_H

#include <QWidget>
#include <QProcess>

class QLabel;
class QLineEdit;
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

    QLabel *m_previewLabel;
    QLineEdit *m_serverUrlLineEdit;
    QLineEdit *m_rtmpNameLineEdit;
    QProcess *m_process;
};

#endif // STREAMWIDGET_H
