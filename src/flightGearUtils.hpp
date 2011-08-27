/*
 * flightGearUtils.hpp
 * Copyright (C) James Goppert 2010 <jgoppert@users.sourceforge.net>
 *
 * flightGearUtils.hpp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * flightGearUtils.hpp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef mavsim_flightGearUtils_HPP
#define mavsim_flightGearUtils_HPP

#include "net_ctrls.hxx"
#include "net_fdm.hxx"
#include <netinet/in.h>

void htond (double &x);
void htonf (float &x);
void htonCtrls( FGNetCtrls & net);
void htonFdm( FGNetFDM & net );

#endif

// vim:ts=4:sw=4

