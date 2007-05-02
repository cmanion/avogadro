/**********************************************************************
  Ghemical - Ghemical Plugin for Avogadro

  Copyright (C) 2006 by Donald Ephraim Curtis
  Copyright (C) 2006 by Geoffrey R. Hutchison

  This file is part of the Avogadro molecular editor project.
  For more information, see <http://avogadro.sourceforge.net/>

  Some code is based on Open Babel
  For more information, see <http://openbabel.sourceforge.net/>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 ***********************************************************************/

#include "ghemical.h"
#include <avogadro/primitive.h>
#include <avogadro/color.h>
#include <avogadro/glwidget.h>

#include <QtGui>

using namespace std;
using namespace OpenBabel;
using namespace Avogadro;

Ghemical::Ghemical() : Extension()
{
  pGhemicalFF = OBForceField::FindForceField("Ghemical");
  
  if (pGhemicalFF) { // make sure we can actually find and run it!
    QAction *action = new QAction(this);
    action->setText("Optimize Geometry");
    m_actions.append(action);
  }
}

Ghemical::~Ghemical() 
{
}

void Ghemical::performAction(QAction *action, Molecule *molecule, QTextEdit *messages)
{
  qDebug() << "Perform Action";

  optimize(molecule, messages);
}

void Ghemical::optimize(Molecule *molecule, QTextEdit *messages)
{
  if (!pGhemicalFF)
    return;

  qDebug() << "Optimize Geometry on " << molecule;

  ostringstream buff;
  pGhemicalFF->SetLogFile(&buff);
  pGhemicalFF->SetLogLevel(OBFF_LOGLVL_LOW);
 
  if (!pGhemicalFF->Setup(*molecule)) {
    qDebug() << "Could not set up force field on " << molecule;
    return;
  }

  pGhemicalFF->ConjugateGradientsInitialize(100, 1e-7f); // initialize cg
  while (pGhemicalFF->ConjugateGradientsTakeNSteps(5)) { // take 5 steps until convergence or 100 steps taken
    pGhemicalFF->UpdateCoordinates(*molecule);
    messages->append(tr(buff.str().c_str()));
    molecule->update();
  }

}

#include "ghemical.moc"
Q_EXPORT_PLUGIN2(ghemical, Ghemical)
