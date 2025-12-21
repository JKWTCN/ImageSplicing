#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QSettings>
#include <imagepro.hpp>
#include <QWheelEvent>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void UpdateQListWidget();
    void ClearDisplay();
    void ClearResult();
    void UnlockPostionButton();
    void LockPostionButton();
    void ReFreshResultWidget();
    // 增强拼接函数
    cv::Mat performEnhancedVerticalStitching(const std::vector<cv::Mat> &images);
    cv::Mat performEnhancedHorizontalStitching(const std::vector<cv::Mat> &images);
    // 自动调整相关函数
    bool performAutoAlignment(const std::vector<cv::Mat> &images, std::vector<cv::Mat> &optimizedImages);
    void createOptimizedVerticalScene(QGraphicsScene *scene, const std::vector<cv::Mat> &images);
    void createOptimizedHorizontalScene(QGraphicsScene *scene, const std::vector<cv::Mat> &images);
    // 智能自动拼接函数
    void performIntelligentAutoStitch();
    cv::Mat performSmartStitching(const std::vector<cv::Mat> &images, bool useVerticalStitching = false);
    cv::Mat performDirectionalMultiStitching(const std::vector<cv::Mat> &images, bool useVerticalStitching);
    std::pair<int, int> findBestStitchingPair(const std::vector<cv::Mat> &images, bool useVerticalStitching);
    double evaluatePairCompatibility(const cv::Mat &img1, const cv::Mat &img2, bool useVerticalStitching);
    double calculateContentSimilarity(const cv::Mat &img1, const cv::Mat &img2);
    cv::Mat stitchPair(const cv::Mat &img1, const cv::Mat &img2, bool useVerticalStitching);
    cv::Mat performSimpleDirectionalStitching(const std::vector<cv::Mat> &images, bool useVerticalStitching);
    int calculateOptimalWidth(const std::vector<cv::Mat> &images);
    int calculateOptimalHeight(const std::vector<cv::Mat> &images);
    cv::Mat resizeToWidth(const cv::Mat &image, int targetWidth);
    cv::Mat resizeToHeight(const cv::Mat &image, int targetHeight);
    cv::Mat applyMandatoryEnhancement(const cv::Mat &image);
    cv::Mat adjustToVertical(const cv::Mat &image);
    cv::Mat adjustToHorizontal(const cv::Mat &image);
    cv::Mat tryVerticalReorganization(const cv::Mat &image);
    cv::Mat tryHorizontalReorganization(const cv::Mat &image);
    cv::Mat tryIntelligentDirectionAdjustment(const cv::Mat &image, bool preferVertical);
    cv::Mat trySmartCropping(const cv::Mat &image, bool preferVertical);
    cv::Mat tryContentAwareResize(const cv::Mat &image, bool preferVertical);
    cv::Mat trySegmentReorganization(const cv::Mat &image, bool preferVertical);
    cv::Mat enhanceStitchedImage(const cv::Mat &image);
    void createAutoStitchScene(QGraphicsScene *scene, const cv::Mat &stitchedResult);
private slots:
    void openFilesBtnPress();
    void upImagePosition();
    void downImagePosition();
    void deleteImagePosition();
    void on_action_about_triggered();
    void on_setting_action_triggered();
    void on_pushButton_horizontalSplicing_clicked();
    void on_pushButton_verticalSplicing_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_auto_clicked();
    void on_pushButton_LayerUp_clicked();
    void on_pushButton_LayerDown_clicked();
    void on_pushButton_copy_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_deleteAll_clicked();
    void on_pushButton_editSelect_clicked();
    void on_pushButton_editResult_clicked();
    bool eventFilter(QObject *obj, QEvent *event); // 事件过滤器
protected:
    // void wheelEvent(QWheelEvent *event) override; // 重写滚轮事件
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
};
#endif // MAINWINDOW_H
