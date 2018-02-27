#include "instrEditor.hpp"

void InstrEditor::loadAlgoPreset(int id, Operator* fmop, Operator* outs)
{
	for (int i = 0; i < 6; i++)
	{
		fmop[i].cleanupBottomLinks();
	}
	switch (id)
	{
		case 0:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[2]);
			break;

		case 1:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[4]);
			fmop[3].connect(&fmop[2], 0, 1);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[2]);
			break;

		case 2:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&fmop[2], 0, 1);
			break;

		case 3:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[1], 0, 1);
			fmop[5].connect(&fmop[2], 0, 1);
			break;

		case 4:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);

			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[5]);

			fmop[5].connect(&fmop[2]);
			fmop[2].connect(&outs[2]);
			break;


		case 5:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);

			fmop[1].connect(&fmop[3]);
			fmop[2].connect(&fmop[3], 0, 1);

			fmop[3].connect(&fmop[4]);
			fmop[3].connect(&fmop[5], 0, 1);

			fmop[4].connect(&outs[2]);
			fmop[5].connect(&outs[3]);
			break;
		case 6:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);
			fmop[0].connect(&fmop[3], 0, 1);
			fmop[0].connect(&fmop[4], 0, 1);
			fmop[1].connect(&fmop[5], 0, 1);

			fmop[2].connect(&fmop[5], 0, 1);
			fmop[3].connect(&fmop[5], 0, 1);
			fmop[4].connect(&fmop[5], 0, 1);
			fmop[5].connect(&outs[1]);
			break;

		case 7:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[3], 0, 1);
			fmop[5].connect(&fmop[3], 0, 1);
			break;
		case 8:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);
			break;

		case 9:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&outs[2]);
			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);
			break;

		case 10:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&outs[2]);
			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[2], 0, 1);
			fmop[5].connect(&outs[4]);
			break;

		case 11:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[5], 0, 1);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&outs[1]);

			fmop[3].connect(&fmop[4]);
			fmop[3].connect(&fmop[2], 0, 1);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);



			break;
		case 12:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);

			fmop[3].connect(&fmop[2]);
			fmop[4].connect(&fmop[2]);

			fmop[5].connect(&fmop[2]);
			fmop[2].connect(&outs[2]);
			break;

		case 13:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);

			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[2]);

			fmop[5].connect(&fmop[4], 0, 1);
			fmop[2].connect(&outs[2]);
			break;

		case 14:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);

			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[2]);

			fmop[5].connect(&fmop[2]);
			fmop[2].connect(&outs[2]);
			break;

		case 15:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[3]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);
			break;

		case 16:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);
			fmop[1].connect(&fmop[3]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);
			break;

		case 17:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&outs[2]);

			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&outs[4]);
			fmop[5].connect(&fmop[4]);
			break;

		case 18:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&outs[1]);

			fmop[2].connect(&fmop[5]);
			fmop[3].connect(&fmop[5]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[3]);
			break;


		case 19:
			fmop[0].connect(&fmop[1]);

			fmop[1].connect(&fmop[2]);
			fmop[1].connect(&fmop[3], 0, 1);
			fmop[1].connect(&fmop[4], 0, 1);
			fmop[1].connect(&fmop[5], 0, 1);

			fmop[2].connect(&outs[1]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&outs[3]);
			fmop[5].connect(&outs[4]);

			break;



		case 20:
			fmop[0].connect(&fmop[4]);
			fmop[1].connect(&fmop[4], 0, 1);

			fmop[2].connect(&fmop[4], 0, 1);
			fmop[3].connect(&fmop[4], 0, 1);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[1]);
			break;

		case 21:
			fmop[0].connect(&fmop[4]);
			fmop[1].connect(&fmop[4], 0, 1);

			fmop[2].connect(&fmop[4], 0, 1);
			fmop[3].connect(&outs[3]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[1]);
			break;

		case 22:
			fmop[0].connect(&fmop[5]);
			fmop[1].connect(&fmop[5], 0, 1);

			fmop[2].connect(&fmop[5], 0, 1);
			fmop[3].connect(&fmop[5], 0, 1);
			fmop[4].connect(&fmop[5], 0, 1);
			fmop[5].connect(&outs[1]);
			break;

		case 23:
			fmop[0].connect(&fmop[5]);
			fmop[1].connect(&fmop[5], 0, 1);

			fmop[2].connect(&fmop[5], 0, 1);
			fmop[3].connect(&fmop[5], 0, 1);
			fmop[4].connect(&outs[3]);
			fmop[5].connect(&outs[1]);
			break;





		case 24:
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);

			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&outs[1]);
			fmop[3].connect(&fmop[2]);
			break;

		case 25:
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);

			fmop[0].connect(&fmop[2]);
			fmop[1].connect(&fmop[2]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[1]);
			break;




		case 26:
			fmop[3].connect(&fmop[4]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[3]);


			fmop[1].connect(&outs[1]);
			fmop[2].connect(&outs[2]);

			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);
			break;

		case 27:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);
			fmop[1].connect(&outs[1]);
			fmop[2].connect(&outs[2]);

			fmop[3].connect(&fmop[5]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);

			break;
		case 28:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&outs[1]);
			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[2]);
			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[3]);
			break;
		case 29:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);
			fmop[0].connect(&fmop[3], 0, 1);

			fmop[1].connect(&outs[1]);
			fmop[2].connect(&outs[2]);
			fmop[3].connect(&outs[3]);

			fmop[4].connect(&fmop[5]);
			fmop[5].connect(&outs[4]);

			break;
		case 30:
			fmop[0].connect(&fmop[1]);
			fmop[0].connect(&fmop[2], 0, 1);
			fmop[0].connect(&fmop[3], 0, 1);

			fmop[1].connect(&outs[1]);
			fmop[2].connect(&outs[2]);
			fmop[3].connect(&outs[3]);

			fmop[4].connect(&outs[4]);
			fmop[5].connect(&outs[5]);
			break;
		case 31:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&outs[1]);

			fmop[2].connect(&fmop[3]);
			fmop[3].connect(&outs[3]);

			fmop[4].connect(&outs[4]);
			fmop[5].connect(&outs[5]);
			break;
		case 32:
			fmop[0].connect(&fmop[2]);
			fmop[2].connect(&outs[1]);

			fmop[1].connect(&fmop[2]);
			fmop[3].connect(&outs[3]);
			fmop[4].connect(&outs[4]);
			fmop[5].connect(&outs[5]);
			break;
		case 33:
			fmop[0].connect(&fmop[1]);
			fmop[1].connect(&outs[0]);

			fmop[2].connect(&outs[2]);
			fmop[3].connect(&outs[3]);
			fmop[4].connect(&outs[4]);
			fmop[5].connect(&outs[5]);
			break;
		case 34:
			fmop[0].connect(&outs[0]);
			fmop[1].connect(&outs[1]);
			fmop[2].connect(&outs[2]);
			fmop[3].connect(&outs[3]);
			fmop[4].connect(&outs[4]);
			fmop[5].connect(&outs[5]);
			break;
	}
	rearrangeOps();
}