/*
Copyright (c) 2015 Robert A. Vazeux Blanco

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 */

#include "../includes/zeus/mailBox.h"

static MailBox_t mailBoxes[MAX_MAILBOXES];

void init_mailBoxes(void) {

	unsigned char i,j;
	for (i = 0; i < MAX_MAILBOXES; ++i) {

		for (j = 0; j < MAX_MESSAGES; ++j) {
			mailBoxes[i].messages[j][0] = '\0';
		}

		mailBoxes[i].nMessages = 0;
		init_queue(&mailBoxes[i].bloqued_procs);

	}
}

void writeMessage(unsigned char nMailBox, unsigned char * messageDir) {

	if(nMailBox >= MAX_MAILBOXES) {
		return;
	}

	if(mailBoxes[nMailBox].nMessages == MAX_MESSAGES) {
		// TODO
		return;
	} else {
		if(mailBoxes[nMailBox].bloqued_procs.head == NULL) {
			strncpy(mailBoxes[nMailBox].messages[mailBoxes[nMailBox].nMessages],messageDir,MAX_MESSAGE_SIZE);
			mailBoxes[nMailBox].nMessages++;
		} else {
			Process_t * waiting_proc = mailBoxes[nMailBox].bloqued_procs.head;
			unsigned char * receive_dir = (unsigned char *) getParameter(waiting_proc->stack_pointer,1);
			strncpy(receive_dir,messageDir,MAX_MESSAGE_SIZE);
			addToReadyQueue(waiting_proc);
			return;
		}

	}

}

unsigned int readMessage(unsigned char nMailBox, unsigned char * messageDir) {

	if(nMailBox >= MAX_MAILBOXES) {
		Process_t * nextProc = getCurrentProcess();
		return nextProc->stack_pointer;
	}

	if(mailBoxes[nMailBox].nMessages == 0) {
		if(mailBoxes[nMailBox].bloqued_procs.head == NULL) {
			mailBoxes[nMailBox].bloqued_procs.head = mailBoxes[nMailBox].bloqued_procs.tail = getCurrentProcess();
		} else {
			mailBoxes[nMailBox].bloqued_procs.tail->nextProc = getCurrentProcess();
			mailBoxes[nMailBox].bloqued_procs.tail = mailBoxes[nMailBox].bloqued_procs.tail->nextProc;
		}
		sConsoleWrite(getCurrentSConsole(),"[PROCESO BLOQUEADO] RAZON: LEER MENSAJE DE BUZON VACIO.");
		Process_t * nextProc = stepReadyQueue();
		return nextProc->stack_pointer;

	} else {
		strncpy(messageDir,mailBoxes[nMailBox].messages[mailBoxes[nMailBox].nMessages],MAX_MESSAGE_SIZE);
		mailBoxes[nMailBox].nMessages--;
	}

	Process_t * nextProc = getCurrentProcess();
	return nextProc->stack_pointer;

}
