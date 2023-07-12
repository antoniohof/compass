using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class SerialController : MonoBehaviour
{

   public static SerialController self;

    UnitySerialPort orientationSensor;

    public bool connected = false;

    private string BufferIn = "";
    private List<string> linesIn = new List<string>();
    private bool FirstLineReceived = false;
    public bool SkipFirstLine = false;
    public int RememberLines = 0;
    public string status = "";

    public Quaternion outputRotation;
    public Vector3 outputRotationEuler;

    void Awake()
    {

        self = this;
    }

    // Use this for initialization
    void Start()
    {
        UnitySerialPort.Instance.OpenSerialPort();
        orientationSensor = UnitySerialPort.Instance;

        outputRotation = new Quaternion(0,0,0,0);
    }

    // Update is called once per frame
    void Update()
    {
        if (orientationSensor != null && Input.GetKeyDown("space")) {
             orientationSensor.OpenSerialPort();
        }
        status = orientationSensor.PortStatus;
        //if (connected) {
            if (orientationSensor != null && orientationSensor.ChunkData != null)
            {
                OnReceivedChunk(orientationSensor.ChunkData);
            }
       // }
    }

    void OnReceivedChunk(string[] chunk)
    {
        Debug.Log("chunk");
        Debug.Log(chunk);

        SendMessage("OnSerialValues", chunk);
    }

    void OnReceivedData(string data)
    {
        Debug.Log(data[0]);
        // prepend pending buffer to received data and split by space
        string[] lines = (BufferIn + data).Split(' ');

        Debug.Log(lines);

        // If last line is not empty, it means the line is not complete (new line did not arrive yet),
        // We keep it in buffer for next data.
        int nLines = lines.Length;
        BufferIn = lines[nLines - 1];

        // Loop until the penultimate line (don't use the last one: either it is empty or it has already been saved for later)
        for (int iLine = 0; iLine < nLines - 1; iLine++)
        {
            string line = lines[iLine];
           // Debug.Log ("Received a line: " + line);

            // skip first line
            if (!FirstLineReceived)
            {
                FirstLineReceived = true;

                if (SkipFirstLine)
                {

                    Debug.Log("First line skipped: " + line);
                    continue;
                }
            }

            // Buffer line
            if (RememberLines > 0)
            {
                linesIn.Add(line);

                // trim lines buffer
                int overflow = linesIn.Count - RememberLines;
                if (overflow > 0)
                {
                    print("Serial removing " + overflow + " lines from lines buffer. Either consume lines before they are lost or set RememberLines to 0.");
                    linesIn.RemoveRange(0, overflow);
                }
            }

            SendMessage("OnSerialLine", line);

            // Notify values

            string[] values = line.Split(',');
            SendMessage("OnSerialValues", values);
        }
    }

    void OnSerialLine(string line)
    {
        Debug.Log("[SERIAL] - received line: " + line);

    }

    void OnSerialValues(string[] values)
    {
        Debug.Log(values[0]);


        if (values.Length == 5)
        {
            float x = float.Parse(values[1]);
            float y = float.Parse(values[2]);
            float z = float.Parse(values[3]);
            float w = float.Parse(values[4]);

            outputRotation = new Quaternion(x,y,z,w);

            outputRotationEuler = outputRotation.eulerAngles;
        }
        else
        {
            Debug.Log("[SERIAL][ERROR] - received incomplete quaternion! " + "size:" + Convert.ToString(values.Length));
        }

    }

}
