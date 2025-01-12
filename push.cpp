/****************************************************************************
** Copyright (C) 2010-2011 Frédéric Logier
** Contact: Frédéric Logier <frederic@logier.org>
**
** https://github.com/nodecast/geekast-cli
**
** This file is part of Nodecast.
**
** Nodecast is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Nodecast is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Nodecast.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#include "push.h"



Push::Push(QObject* parent) : QObject(parent)
{
    //m_auth->setUser("aaa");
    m_network = new QNetworkAccessManager(this);
    m_post_response = "";
    m_http_error = 0;

    m_request.setRawHeader("content-type", "application/xml");

    this->connect( m_network, SIGNAL(finished(QNetworkReply *)),SLOT(slotRequestFinished(QNetworkReply *)));

/*
    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                            SLOT(slotSetProgress(qint64,qint64)));
*/
}


void Push::Payload(QByteArray *ldatas) {

    qDebug() << "payload and pass : " << m_credentials;


    m_request.setRawHeader("Authorization", "Basic " + QByteArray((m_credentials).toAscii()).toBase64());


    // qDebug() << QString::fromAscii(QByteArray(m_request.rawHeader( "Authorization" )).fromBase64());

    qDebug() << m_request.rawHeader( "Authorization" ) << m_credentials.toAscii().toBase64();


    if (m_uuid != "")
    {
        // UPDATE
        url.setUrl(m_server + "/host/update/" + m_uuid);
        //url.setUrl(m_server + "/host/update/");
        qDebug() << "PAYLOAD UPDATE" << m_uuid << " SERVER : " << m_server << "pass : " << m_credentials;
        m_request.setUrl(url);

        m_reply = m_network->put(m_request, *ldatas);
    }
    else
    {
        // CREATE
        qDebug() << "PAYLOAD CREATE" << m_uuid << " SERVER : " << m_server;
        //url.setUrl("http://127.0.0.1:3000/hosts.xml");
        url.setUrl(m_server + "/hosts.xml");
        m_request.setUrl(url);

        m_reply = m_network->post(m_request, *ldatas);
    }


}



Push::~Push() {
    free(m_network);
}



void Push::slotRequestFinished(QNetworkReply *) {

    m_http_error = m_reply->error();
    qDebug() << "HTTP ERROR : " << m_http_error;

    emit httpResponse(m_http_error);

    m_post_response = m_reply->readAll();


    qDebug() << "HTTP RESPONSE : " << m_post_response;

    m_xml_response.setContent(m_post_response);
    m_root = m_xml_response.documentElement();


    m_node = m_root.firstChild();
    m_post_response = m_node.toElement().text();

    if (m_node.toElement().tagName() == "uuid")
    {
        qDebug() << "node : " << m_node.toElement().tagName() << " : " << m_post_response;
        m_uuid = m_post_response;
        emit uuidChanged(m_uuid);
        m_post_response = "";
    }
}
