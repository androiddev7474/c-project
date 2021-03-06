/**
 * Bj�rn Hallstr�m
 * Det grafiska gr�nssnittet - h�r anropas �ven functionerna som ansvarar f�r ber�kkning av Black & Scholes - modellen.
 * 2014-01-06
 */

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "calc.h"
#include "gui.h"

static GtkWidget *entry_a, *entry_s, *entry_v, *entry_t, *entry_r;
static GtkWidget *label_a, *label_s, *label_v, *label_t, *label_r, *label_call, *label_put, *label_call_p, *label_put_p;
static GtkWidget *window, *button;
static GtkWidget *table;


/**
 * Callback-function till button och h�r startar alla ber�kningar.
 */
void entry_Submit(GtkWidget **entry, GtkWidget *widget) {

	GtkWidget *entry_ptr_a = entry[0];
	GtkWidget *entry_ptr_s = entry[1];
	GtkWidget *entry_ptr_t = entry[2];
	GtkWidget *entry_ptr_v = entry[3];
	GtkWidget *entry_ptr_r = entry[4];

	static const gchar *params[5]; // h�mtar de inmatade v�rdena
	params[0] = gtk_entry_get_text(GTK_ENTRY (entry_ptr_a));
	params[1] = gtk_entry_get_text(GTK_ENTRY (entry_ptr_s));
	params[2] = gtk_entry_get_text(GTK_ENTRY (entry_ptr_t));
	params[3] = gtk_entry_get_text(GTK_ENTRY (entry_ptr_v));
	params[4] = gtk_entry_get_text(GTK_ENTRY (entry_ptr_r));

	double paramsD[5]; // flyttalsvektor f�r de inmatade v�rdena.
    int ok = error_check(params, paramsD); // F�r felinmatning. paramsD[] initieras h�r.
    set_params(paramsD[0], paramsD[1], paramsD[2], paramsD[3], paramsD[4]); // skickar parametrar till calc.c f�r ber�kning
	genOptionSer(); // genererar tidsserie med optionspriser f�r samtliga succesiva l�ptidsdagar fram till dagen f�re l�sen.

	double *callArr = getCallArr(); // array med k�poptionspriser
	double *putArr = getPutArr(); // array med s�ljoptionspriser
	int tTM = getMaturity(); // angiven l�ptid

	printCallPutPrices(callArr, putArr, tTM); // anv�nds f�r utskrift av optionspriserna till konsolf�nstret.

    // kodstycket g�r en typecast fr�n double till character-string.
    int len = snprintf(NULL, 0, "%f", callArr[0]); // k�poption
    char *strCall = malloc(len+1);
    snprintf(strCall, len, "%f", callArr[0]);

    len = snprintf(NULL, 0, "%f", putArr[0]); // s�ljoption
	char *strPut = malloc(len+1);
	snprintf(strPut, len, "%f", putArr[0]);

    // uppdatera k�poptionspriset.
    gtk_widget_destroy(GTK_WIDGET(label_call_p));
    label_call_p = gtk_label_new (strCall);
    gtk_grid_attach (GTK_GRID (table), label_call_p, 1, 5, 1, 1);
    gtk_widget_show(label_call_p);

    // uppdatera s�ljoptionspriset.
	gtk_widget_destroy(GTK_WIDGET(label_put_p));
	label_put_p = gtk_label_new (strPut);
	gtk_grid_attach (GTK_GRID (table), label_put_p, 1, 6, 1, 1);
	gtk_widget_show(label_put_p);

	// frig�r minne - det finns ingen garbage-collector ;-(
	free(putArr);
	free(callArr);

	// undvik dangling pointers.
	putArr = NULL;
	callArr = NULL;
}


/**
 * Initierar parameterv�rden av typen double samt kontrollerar ev. felinmatning.
 */
int error_check(const gchar **params, double *paramsD) {

	int valid_input = 1;
	char *err0, *err1, *err2, *err3, *err4;

	int i;
	for (i = 0; i < 5; i++) {
		switch (i) {
			case 0:
				paramsD[0] = strtod(params[0], &err0);
				break;
			case 1:
				paramsD[1] = strtod(params[1], &err1);
				break;
			case 2:
				paramsD[2] = strtod(params[2], &err2);
				break;
			case 3:
				paramsD[3] = strtod(params[3], &err3);
				break;
			case 4:
				paramsD[4] = strtod(params[4], &err4);
				break;
		}
	}

	if (*err0 != 0 || *err1 != 0 ||*err2 != 0 ||*err3 != 0 || *err4 != 0) { // troligtvis ok
		printf("no valid entry\n");
		valid_input = 0;
	} else if (paramsD[2] == 0) {
		printf("no valid entry\n");
		valid_input = 0;
	}

	return valid_input;
}

/**
 * Avslutar f�nster-applikationen.
 */
static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit ();
}


/**
 *
 */
static void initialize_window(GtkWidget* window) {
  gtk_window_set_title(GTK_WINDOW(window),"FINANCIAL CALCULATOR"); //Set window title
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 200); //Default size
  g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL); //avslutar applikation

}

/*
 * H�r skapas det grafiska gr�nssnittet - ett anrop av callback-funktionen entry_submit g�rs via knappen.
 */
int gui () {

  /*Skapar ett nytt f�nster */
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  initialize_window(window);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect_swapped (window, "delete-event", G_CALLBACK (gtk_widget_destroy), window);


   /* Skapar en 1x2 table */
   table = gtk_grid_new ();
   gtk_container_add (GTK_CONTAINER (window), table);

   //Skapar en text box (asset price)
   entry_a = gtk_entry_new ();
   gtk_grid_attach (GTK_GRID (table), entry_a, 0, 0, 1, 1);

   // Skapar en ny label.
   label_a = gtk_label_new (" ASSET PRICE" );
   gtk_grid_attach (GTK_GRID (table), label_a, 1, 0, 1, 1);



   //Skapar en text box text box (strike price)
   entry_s = gtk_entry_new ();
   gtk_grid_attach (GTK_GRID (table), entry_s, 0, 1, 1, 1);

   // Skapar en ny label.
   label_s = gtk_label_new (" STRIKE PRICE" );
   gtk_grid_attach (GTK_GRID (table), label_s, 1, 1, 1, 1);



   //Skapar en text box text box (time to maturity)
   entry_t = gtk_entry_new ();
   gtk_grid_attach (GTK_GRID (table), entry_t, 0, 2, 1, 1);

   // Skapar en ny label.
   label_t = gtk_label_new (" TIME TO MATURITY" );
   gtk_grid_attach (GTK_GRID (table), label_t, 1, 2, 1, 1);




   //Skapar en text box text box (volatility)
   entry_v = gtk_entry_new ();
   gtk_grid_attach (GTK_GRID (table), entry_v, 0, 3, 1, 1);

   // Skapar en ny label.
   label_v = gtk_label_new (" VOLATILITY" );
   gtk_grid_attach (GTK_GRID (table), label_v, 1, 3, 1, 1);



   //Skapar en text box text box (interest rate)
   entry_r = gtk_entry_new ();
   gtk_grid_attach (GTK_GRID (table), entry_r, 0, 4, 1, 1);

   // Skapar en ny label.
   label_r = gtk_label_new (" INTEREST RATE" );
   gtk_grid_attach (GTK_GRID (table), label_r, 1, 4, 1, 1);

   // Skapar en ny label.
  label_call = gtk_label_new (" CALL: " );
  gtk_grid_attach (GTK_GRID (table), label_call, 0, 5, 1, 1);

  // Skapar en ny label f�r det genererade priset.
  label_call_p = gtk_label_new (" ... " );
  gtk_grid_attach (GTK_GRID (table), label_call_p, 1, 5, 1, 1);

  // Skapar en ny label.
  label_put = gtk_label_new (" PUT: " );
  gtk_grid_attach (GTK_GRID (table), label_put, 0, 6, 1, 1);

  // Skapar en ny label f�r det genererade priset.
  label_put_p = gtk_label_new (" ... " );
  gtk_grid_attach (GTK_GRID (table), label_put_p, 1, 6, 1, 1);

   static GtkWidget *entry[5];
   entry[0] = entry_a;
   entry[1] = entry_s;
   entry[2] = entry_t;
   entry[3] = entry_v;
   entry[4] = entry_r;

   button = gtk_button_new_with_label("Calculate");
   g_signal_connect_swapped (button, "clicked", G_CALLBACK (entry_Submit), entry);
   gtk_grid_attach (GTK_GRID (table), button, 0, 7, 2, 1);

   gtk_widget_show_all(window);

  gtk_main ();
  return 2;
}


/**
 * Skriver ut resultaten i konsolf�nstret.
 * double callArr (pekare till array med k�poptionspriser.
 * double putArr (pekare till array med s�ljoptionspriser.
 * int tTM (l�ptiden)
 */
void printCallPutPrices(double *callArr, double *putArr, int tTM) {

	printf("You entered the following values: \n---------------------------------------------------\n");
	//printf("ASSET PRICE: %lf\nSTRIKE PRICE: %lf\nMATURITY: %d\nVOLATILITY: %lf\nINTEREST RATE: %lf\n",
	//		, params.s, params.t, params.v, params.r);
	printf("---------------------------------------------------\n");

	printf("Given these paramteters - the calculated call- and put-prices according to Black&Sholes are shown below\n");
	printf("CALL PRICE: %lf\nPUT PRICE %lf\n", callArr[0], putArr[0]);
	printf("And this are the prices when it is %d tradingdays left of the underlying asset\n", tTM);
	printf("Below are all prices shown for all succesive tradingdays beginning with todays price\n\n");

	int i;
	for (i = 0; i < tTM; i++) {
		printf("%d days left  call: %lf put: %lf\n", tTM - i, callArr[i], putArr[i]);
	}
}
