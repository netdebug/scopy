#ifndef NETWORKANALYZERBUFFERVIEWER_H
#define NETWORKANALYZERBUFFERVIEWER_H

#include <QWidget>
#include <QPushButton>
#include "oscilloscope.hpp"

#include "TimeDomainDisplayPlot.h"

namespace Ui {
class NetworkAnalyzerBufferViewer;
}

struct Buffer {
	Buffer() {}
	Buffer(double frequency, unsigned int sampleRate,
	       unsigned int bufferSize, const std::vector<float> &buffer):
		frequency(frequency), sampleRate(sampleRate),
		bufferSize(bufferSize), buffer(buffer) {}

	double frequency;
	unsigned int sampleRate;
	unsigned int bufferSize;
	std::vector<float> buffer;
};

namespace adiscope {
class NetworkAnalyzerBufferViewer : public QWidget
{
	Q_OBJECT

public:
	explicit NetworkAnalyzerBufferViewer(QWidget *parent = nullptr);
	~NetworkAnalyzerBufferViewer();

	void clear();
	void pushBuffers(QPair<Buffer, Buffer> buffers);
	void selectBuffers(double frequency);
	void setOscilloscope(Oscilloscope *osc);

	QPair<Buffer, Buffer> getSelectedBuffers() const;

protected:
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
	void sendBufferToOscilloscope();

private:
	Ui::NetworkAnalyzerBufferViewer *d_ui;
	TimeDomainDisplayPlot *d_plot;
	QPushButton *d_viewInOsc;
	QVector<QPair<Buffer, Buffer>> d_data;
	int d_selectedBuffersIndex;
	Oscilloscope *d_osc;
	QVector<double> d_currentXdata;
};
}

#endif // NETWORKANALYZERBUFFERVIEWER_H
