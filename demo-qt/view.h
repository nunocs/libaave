/*   This file is part of LibAAVE.
 * 
 *   LibAAVE is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibAAVE is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibAAVE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Copyright 2013 André Oliveira, Nuno Silva, Guilherme Campos,
 *   Paulo Dias, José Vieira/IEETA - Universidade de Aveiro
 *
 *
 *   demo-qt/view.cpp: view of the auralisation world
 */

#include <QWidget>

class View : public QWidget
{
	Q_OBJECT

public:
	View();

protected:
	void keyPressEvent(QKeyEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);

private slots:
	void setOrientation(float, float, float);

private:
	/*
	 * Pointer to the currently selected item (the item being dragged).
	 * It is a struct aave_source * if the currently selected item
	 * is a sound source, or it is a struct aave * if the
	 * currently selected item is the listener.
	 */
	void *selectedItem;

	/* Current orientation (radians). */
	float roll, pitch, yaw;

	/* The bounding box of the room. */
	float xmin, xmax, ymin, ymax, zmin, zmax;

	/*
	 * Values for converting auralisation coordinates to pixels:
	 * px = x0p + (x - x0) * scale;
	 * py = y0p - (y - y0) * scale;
	 */
	float x0p, y0p, x0, y0, scale;

	/* The projected polygons of the surfaces of the room. */
	QVector<QPolygon> polygons;

	/* Auxiliary function to convert a point from 3D to projected 2D. */
	void convert(float xyz[3], int *x, int *y);
};
