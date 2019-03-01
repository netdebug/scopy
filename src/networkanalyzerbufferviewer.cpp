#include "networkanalyzerbufferviewer.h"
#include "ui_networkanalyzerbufferviewer.h"

#include <QDebug>

using namespace adiscope;

NetworkAnalyzerBufferViewer::NetworkAnalyzerBufferViewer(QWidget *parent) :
	QWidget(parent),
	d_ui(new Ui::NetworkAnalyzerBufferViewer),
	d_osc(nullptr)
{
	d_ui->setupUi(this);

	d_plot = new TimeDomainDisplayPlot(d_ui->mainWidget);
	d_ui->mainLayout->addWidget(d_plot);

	d_viewInOsc = new QPushButton("View in Oscilloscope", d_ui->mainWidget);
	d_viewInOsc->setGeometry(width() - 190,
				 height() - 90,
				 160,
				 60);

	connect(d_viewInOsc, &QPushButton::clicked,
		this, &NetworkAnalyzerBufferViewer::sendBufferToOscilloscope);
}

NetworkAnalyzerBufferViewer::~NetworkAnalyzerBufferViewer()
{
	delete d_ui;
}

void NetworkAnalyzerBufferViewer::pushBuffers(QPair<Buffer, Buffer> buffers)
{
	d_data.push_back(buffers);
	qDebug() << d_data.size();
}

void NetworkAnalyzerBufferViewer::selectBuffers(double frequency)
{
	qDebug() << "Display buffers for frequency: " << frequency;

	int index = -1;
	for (int i = 0; i < d_data.size() - 1; ++i) {
		if (d_data[i].first.frequency <= frequency
				&& frequency <= d_data[i + 1].first.frequency) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		return;
	}

	if (index >= d_data.size()) {
		return;
	}


	d_selectedBuffersIndex = index;

	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
	} else {
		d_plot->unregisterReferenceWaveform("data1");
		d_plot->unregisterReferenceWaveform("data2");
	}

	QVector<double> xData;

	double division = 1.0 / d_data[index].first.sampleRate;
	int n = d_data[index].first.buffer.size() / 2;

	for (int i = -n; i < n; ++i) {
		xData.push_back(division * i);
	}

	d_currentXdata.clear();
	d_currentXdata = xData;

	QVector<double> yData1, yData2;
	for (int i = 0; i < d_data[index].first.buffer.size(); ++i) {
		yData1.push_back(d_data[index].first.buffer[i]);
	}
	for (int i = 0; i < d_data[index].second.buffer.size(); ++i) {
		yData2.push_back(d_data[index].second.buffer[i]);
	}

	d_plot->setYaxis(-5, 5);
	d_plot->setXaxis(-n * division, n * division);
	d_plot->registerReferenceWaveform("data1", xData, yData1);
	d_plot->registerReferenceWaveform("data2", xData, yData2);
}

void NetworkAnalyzerBufferViewer::setOscilloscope(Oscilloscope *osc)
{
	d_osc = osc;
}

void NetworkAnalyzerBufferViewer::clear()
{
	d_data.clear();
	//TODO: remove plotted curves
}

void NetworkAnalyzerBufferViewer::resizeEvent(QResizeEvent *event)
{
	d_viewInOsc->setGeometry(width() - 190,
				 height() - 90,
				 160,
				 60);
}

void NetworkAnalyzerBufferViewer::sendBufferToOscilloscope()
{
	d_osc->remove_ref_waveform("NA1");
	d_osc->remove_ref_waveform("NA2");

	qDebug() << "Should open Oscilloscope (detached?) and add the 2 reference waveforms ";
	QVector<double> yData1, yData2;
	auto index = d_selectedBuffersIndex;
	for (int i = 0; i < d_data[index].first.buffer.size(); ++i) {
		yData1.push_back(d_data[index].first.buffer[i]);
	}
	for (int i = 0; i < d_data[index].second.buffer.size(); ++i) {
		yData2.push_back(d_data[index].second.buffer[i]);
	}
	d_osc->add_ref_waveform("NA1", d_currentXdata, yData1);
	d_osc->add_ref_waveform("NA2", d_currentXdata, yData2);

	d_osc->detached();
}
