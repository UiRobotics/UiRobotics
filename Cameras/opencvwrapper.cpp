#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include "opencv2/opencv.hpp"
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;


class myMutex {
	public:
		myMutex() {
			pthread_mutex_init( &m_mutex, NULL );
		}
		void lock() {
			pthread_mutex_lock( &m_mutex );
		}
		void unlock() {
			pthread_mutex_unlock( &m_mutex );
		}
	private:
		pthread_mutex_t m_mutex;
};


class MyFreenectDevice : public Freenect::FreenectDevice {
	public:
		MyFreenectDevice(freenect_context *_ctx, int _index)
	 		: Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
			m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
			m_new_depth_frame(false), depthMat(Size(640,480),CV_16UC1),
			rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
			ownMat(Size(640,480),CV_8UC3,Scalar(0)) {
			
			for( unsigned int i = 0 ; i < 2048 ; i++) {
				float v = i/2048.0;
				v = std::pow(v, 3)* 6;
				m_gamma[i] = v*6*256;
			}
		}
		
		// Do not call directly even in child
		void VideoCallback(void* _rgb, uint32_t timestamp) {
			std::cout << "RGB callback" << std::endl;
			m_rgb_mutex.lock();
			uint8_t* rgb = static_cast<uint8_t*>(_rgb);
			rgbMat.data = rgb;
			m_new_rgb_frame = true;
			m_rgb_mutex.unlock();
		};
		
		// Do not call directly even in child
		void DepthCallback(void* _depth, uint32_t timestamp) {
			std::cout << "Depth callback" << std::endl;
			m_depth_mutex.lock();
			uint16_t* depth = static_cast<uint16_t*>(_depth);
			depthMat.data = (uchar*) depth;
			m_new_depth_frame = true;
			m_depth_mutex.unlock();
		}
		
		bool getVideo(Mat& output) {
			m_rgb_mutex.lock();
			if(m_new_rgb_frame) {
				cv::cvtColor(rgbMat, output, CV_RGB2BGR);
				m_new_rgb_frame = false;
				m_rgb_mutex.unlock();
				return true;
			} else {
				m_rgb_mutex.unlock();
				return false;
			}
		}
		
		bool getDepth(Mat& output) {
				m_depth_mutex.lock();
				if(m_new_depth_frame) {
					depthMat.copyTo(output);
					m_new_depth_frame = false;
					m_depth_mutex.unlock();
					return true;
				} else {
					m_depth_mutex.unlock();
					return false;
				}
			}
	private:
		std::vector<uint8_t> m_buffer_depth;
		std::vector<uint8_t> m_buffer_rgb;
		std::vector<uint16_t> m_gamma;
		Mat depthMat;
		Mat rgbMat;
		Mat ownMat;
		myMutex m_rgb_mutex;
		myMutex m_depth_mutex;
		bool m_new_rgb_frame;
		bool m_new_depth_frame;
};

cv::Mat makeCanvas(std::vector<cv::Mat>& vecMat, int windowHeight, int nRows) {
            int N = vecMat.size();
            nRows  = nRows > N ? N : nRows; 
            int edgeThickness = 10;
            int imagesPerRow = ceil(double(N) / nRows);
            int resizeHeight = floor(2.0 * ((floor(double(windowHeight - edgeThickness) / nRows)) / 2.0)) - edgeThickness;
            int maxRowLength = 0;

            std::vector<int> resizeWidth;
            for (int i = 0; i < N;) {
                    int thisRowLen = 0;
                    for (int k = 0; k < imagesPerRow; k++) {
                            double aspectRatio = double(vecMat[i].cols) / vecMat[i].rows;
                            int temp = int( ceil(resizeHeight * aspectRatio));
                            resizeWidth.push_back(temp);
                            thisRowLen += temp;
                            if (++i == N) break;
                    }
                    if ((thisRowLen + edgeThickness * (imagesPerRow + 1)) > maxRowLength) {
                            maxRowLength = thisRowLen + edgeThickness * (imagesPerRow + 1);
                    }
            }
            int windowWidth = maxRowLength;
            cv::Mat canvasImage(windowHeight, windowWidth, CV_8UC3, Scalar(0, 0, 0));

            for (int k = 0, i = 0; i < nRows; i++) {
                    int y = i * resizeHeight + (i + 1) * edgeThickness;
                    int x_end = edgeThickness;
                    for (int j = 0; j < imagesPerRow && k < N; k++, j++) {
                            int x = x_end;
                            cv::Rect roi(x, y, resizeWidth[k], resizeHeight);
                            cv::Size s = canvasImage(roi).size();
                            // change the number of channels to three
                            cv::Mat target_ROI(s, CV_8UC3);
                            if (vecMat[k].channels() != canvasImage.channels()) {
                                if (vecMat[k].channels() == 1) {
                                    cv::cvtColor(vecMat[k], target_ROI, CV_GRAY2BGR);
                                }
                            } else {             
                                vecMat[k].copyTo(target_ROI);
                            }
                            cv::resize(target_ROI, target_ROI, s);
                            if (target_ROI.type() != canvasImage.type()) {
                                target_ROI.convertTo(target_ROI, canvasImage.type());
                            }
                            target_ROI.copyTo(canvasImage(roi));
                            x_end += resizeWidth[k] + edgeThickness;
                    }
            }
            return canvasImage;
    }

int main(int argc, char **argv) {
	bool die(false);
	string filename("snapshot");
	string suffix(".png");
	int i_snap(0),iter(0);
	int lowThreshold;
	int const max_lowThreshold = 100;
	int ratio = 3;
	int kernel_size = 3;
	
	VideoCapture webcam1(0);
	VideoCapture webcam2(1);
	
	
	Mat depthMat(Size(640,480),CV_16UC1);
	Mat depthf (Size(640,480),CV_8UC1);
	Mat wcframe1 (Size(640,480),CV_8UC3,Scalar(0));
	Mat wcframe2 (Size(640,480),CV_8UC3,Scalar(0));
	Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));
        Mat depthEdges (Size(640,480),CV_16UC1);
	
	vector<Mat> images;
	images.push_back(depthf);
	images.push_back(rgbMat);
	images.push_back(wcframe1);
	images.push_back(wcframe2);

	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:
	
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
	
	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
        createTrackbar( "Min Threshold:", "depth", &lowThreshold, max_lowThreshold);
	device.startVideo();
	device.startDepth();
	while (!die) {
		device.getVideo(rgbMat);
		device.getDepth(depthMat);
		webcam1 >> wcframe1;
		webcam2 >> wcframe2;
		depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
		cv::imshow("rgb",makeCanvas(images, 480, 2));
		char k = cvWaitKey(5);
		if( k == 27 ){
			cvDestroyWindow("rgb");
			cvDestroyWindow("depth");
			break;
		}
		if( k == 8 ) {
			std::ostringstream file;
			file << filename << i_snap << suffix;
			cv::imwrite(file.str(),rgbMat);
			i_snap++;
		}
		if(iter >= 10000) break;
		iter++;
	}
	
	device.stopVideo();
	device.stopDepth();
	return 0;
}
