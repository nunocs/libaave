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
 *   demo-qvtk/audio.h: audio output thread
 */

#include <QThread>

class Audio : public QThread
{
public:
	Audio(struct aave *);

protected:
	void run();

private:
	struct aave *aave;
};
