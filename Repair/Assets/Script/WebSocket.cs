using System;
using WebSocketSharp;
using WebSocketSharp.Server;
using UnityEngine;
using UnityEngine.Serialization;
using Random = UnityEngine.Random;

public class WebSocket : MonoBehaviour
{
    public int port = 8989;

    public GameObject paddleTracker;

    // Start is called before the first frame update
    private WebSocketServer wssv;
    private string temperature;
    private PaddleWSHandler PaddleWsHnd { get; set; }
    private EchoWSHandler EchoWsHnd { get; set; }

    private void Start()
    {
        UnityThread.initUnityThread(); //UnityThread is copied from internet and is used as is.
    }

    private void OnEnable()
    {
        wssv = new WebSocketServer(port);

        PaddleWsHnd = new PaddleWSHandler(paddleTracker);
        EchoWsHnd = new EchoWSHandler(paddleTracker);
        wssv.AddWebSocketService<PaddleWSHandler>("/paddle", () => PaddleWsHnd);
        wssv.AddWebSocketService<EchoWSHandler>("/corona", () => EchoWsHnd);

        wssv.Start();

        Debug.LogFormat("Websocket Server started on port {0}", port);
    }
    
    private class EchoWSHandler : WebSocketBehavior
    {
        private PaddleTracker _tracker;

        public EchoWSHandler(GameObject tracker)
        {
            _tracker = tracker.GetComponent<PaddleTracker>();
        }
         protected override void OnClose(CloseEventArgs e)
        {
            Debug.Log("OnClose()");
            base.OnClose(e);
        }

        protected override void OnError(ErrorEventArgs e)
        {
            Debug.LogFormat("OnError({0})", e);
            base.OnError(e);
        }

        protected override void OnOpen()
        {
            Debug.Log("OnOpen()");

            base.OnOpen();
        }

        protected override void OnMessage(MessageEventArgs e)
        {
            var buffer = e.RawData;
            // Debug.LogFormat("Got message from the websocket value'{0}'", BitConverter.ToString(buffer));
            //
            // Debug.LogFormat("The byte buffer is {0} length", buffer.Length);
            /*
                struct sensorData_t{
                  float quatI;
                  float quatJ;
                  float quatK;
                  float quatReal;
                  float hula_speed;
                  float hula_diameter;
                  float hula_count;
                };             */
            //float qx = BitConverter.ToSingle(buffer, 0);
            //float qy = BitConverter.ToSingle(buffer, 4);
            //float qz = BitConverter.ToSingle(buffer, 8);
            //float qw = BitConverter.ToSingle(buffer, 12);
            float hula_speed = BitConverter.ToSingle(buffer, 16);
            float hula_diameter = BitConverter.ToSingle(buffer, 20);
            float hula_count = BitConverter.ToSingle(buffer, 24);

            /* Debug.LogFormat(
                 "Got message from the websocket \nqx:'{0}' qy:'{1}' qz:'{2}' qw:'{3}' hula_s:'{4}' hula_d:'{5}' hula_c:'{6}'",
                 qx, qy,
                 qz, qw,
                 hula_speed,hula_diameter,hula_count);
             */
             Debug.LogFormat(
                  "Got message from the websocket \n hula_s:'{0}' hula_d:'{1}' hula_c:'{2}'",
                  hula_speed,hula_diameter,hula_count);
              
           //https://forums.adafruit.com/viewtopic.php?t=81671
           // _tracker.setNewPosition(new Quaternion(-qw, -qy, -qz, qx));
          // _tracker.setNewPosition(new Quaternion(qx, qy, qz, qw)); // BEST FIT !!

        }
    }

    private class PaddleWSHandler : WebSocketBehavior
    {
        private PaddleTracker _tracker;

        public PaddleWSHandler(GameObject tracker)
        {
            _tracker = tracker.GetComponent<PaddleTracker>();
        }

        public void sendto(string msg)
        {
            Send(msg);
        }

        protected override void OnClose(CloseEventArgs e)
        {
            Debug.Log("OnClose()");
            base.OnClose(e);
        }

        protected override void OnError(ErrorEventArgs e)
        {
            Debug.LogFormat("OnError({0})", e);
            base.OnError(e);
        }

        protected override void OnOpen()
        {
            Debug.Log("OnOpen()");

            base.OnOpen();
        }

        protected override void OnMessage(MessageEventArgs e)
        {
            var buffer = e.RawData;
            // Debug.LogFormat("Got message from the websocket value'{0}'", BitConverter.ToString(buffer));
            //
            // Debug.LogFormat("The byte buffer is {0} length", buffer.Length);
            /*
                  struct sensorData_t{
                  float quatI;
                  float quatJ;
                  float quatK;
                  float quatReal;
                  uint8_t mostLikelyActivity;
                  uint8_t stabilityClass;
                  uint16_t jx;
                  uint16_t jy;
                  uint16_t jz;
                  uint16_t steps;
                  float power;
                };
             */
            float qx = BitConverter.ToSingle(buffer, 0);
            float qy = BitConverter.ToSingle(buffer, 4);
            float qz = BitConverter.ToSingle(buffer, 8);
            float qw = BitConverter.ToSingle(buffer, 12);
            byte activity = buffer[16];
            byte stability = buffer[17];
            ushort jx = BitConverter.ToUInt16(buffer, 18);
            ushort jy = BitConverter.ToUInt16(buffer, 20);
            ushort jz = BitConverter.ToUInt16(buffer, 22);
            ushort steps = BitConverter.ToUInt16(buffer, 24);
            float power = BitConverter.ToSingle(buffer, 26);

            // Debug.LogFormat(
            //     "Got message from the websocket \nqx:'{0}' qy:'{1}' qz:'{2}' qw:'{3}'\nactivity:'{4}' stability:'{5}'\n jx:'{6}' jy:'{7}' jz:'{8}'\nsteps:'{9}' power:'{10}'",
            //     qx, qy,
            //     qz, qw, activity, stability, jx, jy, jz, steps, power);

           //https://forums.adafruit.com/viewtopic.php?t=81671
            _tracker.setNewPosition(new Quaternion(-qw, -qy, -qz, qx));
            _tracker.setjoystick(jx, jy, jz);
            
            // Debug.LogFormat("Got message from the websocket temperature? '{0}'", BitConverter.ToInt16(buffer,4));
            
        }
    }
}