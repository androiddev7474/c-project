/*
 * gui.h
 *
 *  Created on: 6 jan 2014
 *      Author: �garen
 */

#ifndef GUI_H_
#define GUI_H_

int error_check(const gchar **params, double *paramsD); // initierar flyttalsparametrarna f�r parametrarna (vid inmating text) samt felkontroll.
void printCallPutPrices(double *callArr, double *putArr, int tTM); // skriver ut samtliga k�p- och s�ljoptionspriser i konsolf�nstret

#endif /* GUI_H_ */
