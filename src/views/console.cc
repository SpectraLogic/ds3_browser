/*
 * *****************************************************************************
 *   Copyright 2014 Spectra Logic Corporation. All Rights Reserved.
 *   Licensed under the Apache License, Version 2.0 (the "License"). You may not
 *   use this file except in compliance with the License. A copy of the License
 *   is located at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   or in the "license" file accompanying this file.
 *   This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *   CONDITIONS OF ANY KIND, either express or implied. See the License for the
 *   specific language governing permissions and limitations under the License.
 * *****************************************************************************
 */

#include "lib/logger.h"
#include "views/console.h"

Console* Console::s_instance = 0;

Console*
Console::Instance()
{
	if (!s_instance) {
		s_instance = new Console();
		Logger::Instance()->SetStream(s_instance->m_text);
	}
	return s_instance;
}

Console::Console(QWidget* parent)
	: QWidget(parent)
{
	m_text = new QTextEdit();
	m_text->setReadOnly(true);
	m_layout = new QVBoxLayout(this);
	m_layout->addWidget(m_text);
	setLayout(m_layout);
}
