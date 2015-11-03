/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

using namespace bb::cascades;

void  ApplicationUI::httpTest() {
    QNetworkRequest request = QNetworkRequest();
    request.setUrl(QUrl("http://10.3.0.1:8001/scenes"));

    // Create the network access manager

    QNetworkAccessManager* m_pNetAccessMngr = new QNetworkAccessManager(this);

    // Connect your custom slot to its finished() signal
    // and check the return value for connection errors
    bool res;
    Q_UNUSED(res);

    res = connect(m_pNetAccessMngr,
                  SIGNAL(finished(QNetworkReply*)),
                  this,
                  SLOT(onRequestFinished()));
    Q_ASSERT(res);

    // Use the QNetworkAccessManager object to send the HTTP
    // request and capture the reply

    m_pNetReply = m_pNetAccessMngr->get(request);
}

void ApplicationUI::onRequestFinished() {
    // Check for errors and verify that data is available

    if (m_pNetReply != NULL &&
            m_pNetReply->bytesAvailable() > 0 &&
            m_pNetReply->error() == QNetworkReply::NoError) {
        QString jsonData = m_pNetReply->readAll();
        emit labelStringChanged(jsonData);
        m_pNetReply->deleteLater();
    } else {
        // Handle errors here
    }
}

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("applicationUI", this);

    // Create root object for the UI
    root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

QString ApplicationUI::getLabelString() const {
    return QString("Hello Justin");
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("HelloWorld_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}
