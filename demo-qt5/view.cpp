/*
 * demo-qt5/view.cpp: view of the auralisation world
 *
 * Copyright 2013 Universidade de Aveiro
 *
 * Funded by FCT project AcousticAVE (PTDC/EEA-ELC/112137/2009)
 *
 * Written by Andre B. Oliveira <abo@ua.pt>
 */

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include "audio.h"
#include "geometry.h"
#include "imu.h"
#include "view.h"

#include <math.h> /* M_PI */

extern "C" {
#include <aave.h>
}

View::View()
	: QWidget()
	, selectedItem(0)
	, roll(0)
	, pitch(0)
	, yaw(0)
{
	setFocusPolicy(Qt::StrongFocus);
	setMinimumSize(400, 250);

	/* Create and initialise the auralisation engine. */
	aave = (struct aave *)malloc(sizeof *aave);
	aave_init(aave);

	/* Select the HRTF set to use. */
	aave_hrtf_mit(aave);
	/* aave_hrtf_cipic(aave); */
	/* aave_hrtf_listen(aave); */
	/* aave_hrtf_tub(aave); */

	/* Read room model. */
	aave_read_obj(aave, "model.obj");

	/* Set initial position and orientation of the listener. */
	aave_set_listener_position(aave, -4, -2, 1);
	aave_set_listener_orientation(aave, roll, pitch, yaw);

	/* Create one sound source. */
	struct aave_source *source;
	source = (struct aave_source *)malloc(sizeof *source);
	aave_init_source(aave, source);
	aave_add_source(aave, source);
	aave_set_source_position(source, 4, 2, 1);

	/* Calculate the boundary box of the room. */
	float c;
	struct aave_surface *surface;
	xmin = ymin = zmin = +9999;
	xmax = ymax = zmax = -9999;
	for (surface = aave->surfaces; surface; surface = surface->next) {
		for (unsigned i = 0; i < surface->npoints; i++) {
			c = surface->points[i][0];
			if (c > xmax)
				xmax = c;
			if (c < xmin)
				xmin = c;
			c = surface->points[i][1];
			if (c > ymax)
				ymax = c;
			if (c < ymin)
				ymin = c;
			c = surface->points[i][2];
			if (c > zmax)
				zmax = c;
			if (c < zmin)
				zmin = c;
		}
	}

	/* The world point that goes to the center of the widget. */
	x0 = (xmax + xmin) / 2;
	y0 = (ymax + ymin) / 2;

	/*
	 * Timer to repaint the widget, to reflect the incremental changes
	 * being made by the geometry thread and the IMU thread.
	 */
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(update()));
	timer->start(200); /* 5Hz */

	/* Create object to handle the auralisation geometry updates. */
	new Geometry(aave);

	/* Create object to handle the audio output. */
	new Audio(aave);

	/* Create object to handle the head tracker. */
	Imu *imu = new Imu;
	connect(imu, SIGNAL(orientation(float,float,float)),
		SLOT(setOrientation(float,float,float)));
}

/*
 * Convert point [x,y,z] in auralisation world coordinates
 * to pixel coordinates.
 */
void View::convert(float xyz[3], int *x, int *y)
{
	float zscale = (1 + (xyz[2] - zmin) / (zmax - zmin) * 0.3) * scale;
	*x = x0p + (xyz[0] - x0) * zscale;
	*y = y0p - (xyz[1] - y0) * zscale;
}

void View::keyPressEvent(QKeyEvent *event)
{
	float x;

	switch (event->key()) {
	case Qt::Key_0:
		aave->reflections = 0;
		break;
	case Qt::Key_1:
		aave->reflections = 1;
		break;
	case Qt::Key_2:
		aave->reflections = 2;
		break;
	case Qt::Key_3:
		aave->reflections = 3;
		break;
	case Qt::Key_4:
		aave->reflections = 4;
		break;
	case Qt::Key_5:
		aave->reflections = 5;
		break;
	case Qt::Key_6:
		aave->reflections = 6;
		break;
	case Qt::Key_7:
		aave->reflections = 7;
		break;
	case Qt::Key_8:
		aave->reflections = 8;
		break;
	case Qt::Key_9:
		aave->reflections = 9;
		break;
	case Qt::Key_Left:
		x = yaw - (M_PI / 180 * 5);
		if (x < -M_PI)
			x += 2 * M_PI;
		yaw = x;
		aave_set_listener_orientation(aave, roll, pitch, yaw);
		break;
	case Qt::Key_Right:
		x = yaw + (M_PI / 180 * 5);
		if (x > M_PI)
			x -= 2 * M_PI;
		yaw = x;
		aave_set_listener_orientation(aave, roll, pitch, yaw);
		break;
	default:
		QWidget::keyPressEvent(event);
		return;
	}

	update();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
	float x, y, z, zscale;

	if (!selectedItem)
		return;

	if (selectedItem == aave) {
		z = aave->position[2];
		zscale = (1 + (z - zmin) / (zmax - zmin) * 0.3) * scale;
		x = x0 + (event->x() - x0p) / zscale;
		y = y0 - (event->y() - y0p) / zscale;
		aave_set_listener_position(aave, x, y, z);
	} else {
		struct aave_source *source = (struct aave_source *)selectedItem;
		z = source->position[2];
		zscale = (1 + (z - zmin) / (zmax - zmin) * 0.3) * scale;
		x = x0 + (event->x() - x0p) / zscale;
		y = y0 - (event->y() - y0p) / zscale;
		aave_set_source_position(source, x, y, z);
	}

	update();
}

void View::mousePressEvent(QMouseEvent *event)
{
	struct aave_source *source;
	int x, y, dx, dy;

	/* See if the pointer is on the listener. */
	convert(aave->position, &x, &y);
	dx = event->x() - x;
	dy = event->y() - y;
	if (dx * dx + dy * dy < 32) {
		selectedItem = aave;
		return;
	}

	/* See if the pointer is on any of the sound sources. */
	for (source = aave->sources; source; source = source->next) {
		convert(source->position, &x, &y);
		dx = event->x() - x;
		dy = event->y() - y;
		if (dx * dx + dy * dy < 32) {
			selectedItem = source;
			return;
		}
	}

	/* The pointer is not on the listener or sound sources. */
	selectedItem = 0;
}

void View::paintEvent(QPaintEvent *event)
{
	int x, y;

	QPainter painter(this);

	/* White background. */
	painter.fillRect(event->rect(), Qt::white);

	/* Draw sound paths. */
	struct aave_sound *snd;
	for (unsigned k = 0; k <= aave->reflections; k++) {
		unsigned i = aave->reflections - k;
		static const QPen pens[] = {
			QPen(Qt::red),
			QPen(Qt::blue),
			QPen(Qt::green),
			QPen(Qt::cyan),
			QPen(Qt::magenta),
			QPen(Qt::yellow),
			QPen(Qt::black),
			QPen(Qt::gray),
		};
		painter.setPen(pens[i & 7]);
		for (snd = aave->sounds[i]; snd; snd = snd->next) {
			if (!snd->flags)
				continue;
			float *a = snd->source->position;
			for (unsigned j = 0; j < i; j++) {
				float *b = snd->reflection_points[j];
				int x1, y1, x2, y2;
				convert(a, &x1, &y1);
				convert(b, &x2, &y2);
				painter.drawLine(x1, y1, x2, y2);
				a = b;
			}
			int x1, y1, x2, y2;
			convert(a, &x1, &y1);
			convert(aave->position, &x2, &y2);
			painter.drawLine(x1, y1, x2, y2);
		}
	}

	/* Draw surfaces (projected polygons). */
	static const QPen polygonPen(Qt::black);
	painter.setPen(polygonPen);
	for (int i = 0; i < polygons.size(); i++)
		painter.drawPolygon(polygons.at(i));

	/* Draw sound sources. */
	static const QPixmap sourcePixmap("source.png");
	for (struct aave_source *s = aave->sources; s; s = s->next) {
		convert(s->position, &x, &y);
		x -= sourcePixmap.width() / 2;
		y -= sourcePixmap.height() / 2;
		painter.drawPixmap(x, y, sourcePixmap);
	}

	/* Draw listener. */
	static const QImage listenerImage("listener.png");
	QTransform transform;
	transform.rotate(yaw * (180 / M_PI));
	QImage image = listenerImage.transformed(transform,
						Qt::SmoothTransformation);
	convert(aave->position, &x, &y);
	x -= image.width() / 2;
	y -= image.height() / 2;
	painter.drawImage(x, y, image);
}

void View::resizeEvent(QResizeEvent * /* event */)
{
	struct aave_surface *surface;
	int x, y;

	/* The pixel at the center of the widget. */
	x0p = width() * 0.5;
	y0p = height() * 0.5;

	/* The scale to apply (constant width/height ratio). */
	float xscale = width() / (xmax - xmin);
	float yscale = height() / (ymax - ymin);
	scale = xscale > yscale ? yscale : xscale;

	/* Leave 22% for altitude and 2% for margins. */
	scale *= 0.76;

	/* Calculate the polygon coordinates for the new window size. */
	polygons.clear();
	for (surface = aave->surfaces; surface; surface = surface->next) {
		QPolygon polygon(surface->npoints);
		for (unsigned i = 0; i < surface->npoints; i++) {
			convert(surface->points[i], &x, &y);
			polygon.setPoint(i, x, y);
		}
		polygons.append(polygon);
	}
}

void View::setOrientation(float roll, float pitch, float yaw)
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
	aave_set_listener_orientation(aave, roll, pitch, yaw);
}
