/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @constructor
 * @extends {WebInspector.SDKObject}
 * @param {!WebInspector.Target} target
 */
WebInspector.ServiceWorkerManager = function(target)
{
    WebInspector.SDKObject.call(this, target);
    target.registerServiceWorkerDispatcher(new WebInspector.ServiceWorkerDispatcher(this));
    this._lastAnonymousTargetId = 0;
    /** @type {!Map.<string, !WebInspector.ServiceWorker>} */
    this._workers = new Map();
    this.enable();
}

WebInspector.ServiceWorkerManager.Events = {
    WorkersUpdated: "WorkersUpdated"
}

WebInspector.ServiceWorkerManager.prototype = {
    enable: function()
    {
        if (this._enabled)
            return;
        this._enabled = true;

        this.target().serviceWorkerAgent().enable();
        WebInspector.targetManager.addEventListener(WebInspector.TargetManager.Events.MainFrameNavigated, this._mainFrameNavigated, this);
    },

    disable: function()
    {
        if (!this._enabled)
            return;
        this._enabled = false;

        for (var worker of this._workers.values())
            worker._connection.close();
        this._workers.clear();
        this.target().serviceWorkerAgent().disable();
        WebInspector.targetManager.removeEventListener(WebInspector.TargetManager.Events.MainFrameNavigated, this._mainFrameNavigated, this);
    },

    /**
     * @return {!Iterator.<!WebInspector.ServiceWorker>}
     */
    workers: function()
    {
        return this._workers.values();
    },

    /**
     * @return {boolean}
     */
    hasWorkers: function()
    {
        return !!this._workers.size;
    },

    /**
     * @param {string} workerId
     * @param {string} url
     */
    _workerCreated: function(workerId, url)
    {
        new WebInspector.ServiceWorker(this, workerId, url);
    },

    /**
     * @param {string} workerId
     */
    _workerTerminated: function(workerId)
    {
        var worker = this._workers.get(workerId);
        if (!worker)
            return;

        worker._closeConnection();
        this._workers.delete(workerId);

        this.dispatchEventToListeners(WebInspector.ServiceWorkerManager.Events.WorkersUpdated);
    },

    /**
     * @param {string} workerId
     * @param {string} message
     */
    _dispatchMessage: function(workerId, message)
    {
        var worker = this._workers.get(workerId);
        if (worker)
            worker._connection.dispatch(message);
    },

    /**
     * @param {!WebInspector.Event} event
     */
    _mainFrameNavigated: function(event)
    {
        // Attach to the new worker set.
    },

    __proto__: WebInspector.SDKObject.prototype
}

/**
 * @constructor
 * @param {!WebInspector.ServiceWorkerManager} manager
 * @param {string} workerId
 * @param {string} url
 */
WebInspector.ServiceWorker = function(manager, workerId, url)
{
    this._manager = manager;
    this._agent = manager.target().serviceWorkerAgent();
    this._workerId = workerId;
    this._connection = new WebInspector.ServiceWorkerConnection(this._agent, workerId);

    var parsedURL = url.asParsedURL();
    this._name = parsedURL ? parsedURL.lastPathComponent : "#" + (++WebInspector.ServiceWorker._lastAnonymousTargetId);
    this._scope = parsedURL.host + parsedURL.folderPathComponents;
    var title = WebInspector.UIString("\u2699 %s", this._name);

    this._manager._workers.set(workerId, this);
    WebInspector.targetManager.createTarget(title, WebInspector.Target.Type.ServiceWorker, this._connection, manager.target(), targetCreated.bind(this));

    /**
     * @param {?WebInspector.Target} target
     * @this {WebInspector.ServiceWorker}
     */
    function targetCreated(target)
    {
        if (!target) {
            this._manager._workers.delete(workerId);
            return;
        }
        this._manager.dispatchEventToListeners(WebInspector.ServiceWorkerManager.Events.WorkersUpdated);
        target.runtimeAgent().run();
    }
}

WebInspector.ServiceWorker._lastAnonymousTargetId = 0;

WebInspector.ServiceWorker.prototype = {
    /**
     * @return {string}
     */
    name: function()
    {
        return this._name;
    },

    /**
     * @return {string}
     */
    scope: function()
    {
        return this._scope;
    },

    stop: function()
    {
        this._agent.stop(this._workerId);
    },

    _closeConnection: function()
    {
        this._connection._close();
    }
}

/**
 * @constructor
 * @implements {ServiceWorkerAgent.Dispatcher}
 * @param {!WebInspector.ServiceWorkerManager} manager
 */
WebInspector.ServiceWorkerDispatcher = function(manager)
{
    this._manager = manager;
}

WebInspector.ServiceWorkerDispatcher.prototype = {
    /**
     * @override
     * @param {string} workerId
     * @param {string} url
     */
    workerCreated: function(workerId, url)
    {
        this._manager._workerCreated(workerId, url);
    },

    /**
     * @override
     * @param {string} workerId
     */
    workerTerminated: function(workerId)
    {
        this._manager._workerTerminated(workerId);
    },

    /**
     * @override
     * @param {string} workerId
     * @param {string} message
     */
    dispatchMessage: function(workerId, message)
    {
        this._manager._dispatchMessage(workerId, message);
    },

    /**
     * @override
     * @param {!Array.<!ServiceWorkerAgent.ServiceWorkerRegistration>} registrations
     */
    workerRegistrationUpdated: function(registrations)
    {
        //FIXME: implement this.
    },

    /**
     * @override
     * @param {!Array.<!ServiceWorkerAgent.ServiceWorkerVersion>} versions
     */
    workerVersionUpdated: function(versions)
    {
        //FIXME: implement this.
    },

    /**
     * @override
     * @param {string} registrationId
     */
    workerRegistrationDeleted: function(registrationId)
    {
        //FIXME: implement this.
    }
}

/**
 * @constructor
 * @extends {InspectorBackendClass.Connection}
 * @param {!Protocol.ServiceWorkerAgent} agent
 * @param {string} workerId
 */
WebInspector.ServiceWorkerConnection = function(agent, workerId)
{
    InspectorBackendClass.Connection.call(this);
    //FIXME: remove resourceTreeModel and others from worker targets
    this.suppressErrorsForDomains(["Worker", "Page", "CSS", "DOM", "DOMStorage", "Database", "Network", "IndexedDB", "ServiceWorkerCache"]);
    this._agent = agent;
    this._workerId = workerId;
}

WebInspector.ServiceWorkerConnection.prototype = {
    /**
     * @override
     * @param {!Object} messageObject
     */
    sendMessage: function(messageObject)
    {
        this._agent.sendMessage(this._workerId, JSON.stringify(messageObject));
    },

    _close: function()
    {
        this.connectionClosed("worker_terminated");
    },

    __proto__: InspectorBackendClass.Connection.prototype
}
