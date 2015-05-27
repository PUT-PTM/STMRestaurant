using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MySql.Data.MySqlClient;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
        private string MyConString = "SERVER=mysql3.superhost.pl;" +
                "DATABASE=sh188724_db;" +
                "UID=sh188724_user;" +
                "PASSWORD=dzkpbAZS";

        System.IO.Ports.SerialPort serP;
        public Form1()
        {
            InitializeComponent();

            textBox2.AppendText("Dostępne porty:\n");
            foreach (string s in System.IO.Ports.SerialPort.GetPortNames())
            {
                textBox2.AppendText(s + "\n");
            }

            getMySql();
        }

        private List<string> orders = new List<string>();
        string[] menu = new string[] { 
            "devolay z szynką i serem", 
            "filet drobiowy i bukiet surowek", 
            "stek wolowy z talarkami ziemniaczanymi", 
            "shoarma z kurczaka", 
            "losos na szpinaku", 
            "salatka cezar z kurczakiem", 
            "watrobka z pieczarkami", 
            "ACCEPT", 
            "CANCEL" 
        };

        void timer_Tick(object sender, EventArgs e)
        {

            if (serP.BytesToRead > 0)
            {
                int msg = serP.ReadChar();

                if (msg < 100 && msg <= menu.Length - 1 && msg >= 0)
                    if (menu[msg] == "ACCEPT")
                    {
                        if (orders.Count != 0)
                        {
                            insertToMySql();
                            getMySql();
                            orders.Clear();
                        }
                        else
                        {
                            textBox3.Text = "lista zamówień jest pusta";
                        }
                    }
                    else if (menu[msg] == "CANCEL")
                    {
                        orders.Clear();
                    }
                    else
                        orders.Add(menu[msg]);
                else if (msg >= 100 && msg - 100 <= menu.Length - 1 && msg - 100 >= 0)
                {
                    if (menu[msg - 100] == "ACCEPT") { ;}
                    else if (menu[msg - 100] == "CANCEL") { ;}
                    else
                        orders.Remove(menu[msg - 100]);
                }
                else
                    textBox3.Text = "błąd - nie ma w bazie dania o takim indeksie";
            }
        }
        private string implodeOrders()
        {
            string sklejka = "";
            foreach (string val in orders)
            {
                sklejka += val + ",\n ";
            }
            return sklejka;
        }

        private void getMySql()
        {
            try
            {
                MySqlConnection connection = new MySqlConnection(MyConString);
                MySqlCommand command = connection.CreateCommand();

                connection.Open();

                command.CommandText = "select * from restaurant";
                MySqlDataReader Reader = command.ExecuteReader();

                DataTable table = new DataTable();
                table.Load(Reader);
                dataGridView1.DataSource = table;

                if (table.Rows.Count != 0)
                {
                    DataRow row = table.Rows[0];
                    DataColumn column = table.Columns[0];
                }

                connection.Close();

            }
            catch (MySqlException ex)
            {
                textBox3.Text = (ex.ToString());
            }

        }

        private void insertToMySql()
        {
            try
            {
                MySqlConnection connection = new MySqlConnection(MyConString);
                MySqlCommand command = connection.CreateCommand();

                connection.Open();


                command.CommandText = "INSERT INTO restaurant(orderList, nrOfTable) VALUES(@orderList, @nrOfTable)";
                command.Prepare();

                command.Parameters.AddWithValue("@orderList", implodeOrders());
                command.Parameters.AddWithValue("@nrOfTable", 1);
                command.ExecuteNonQuery();
                connection.Close();
            }
            catch (MySqlException ex)
            {
                textBox3.Text = (ex.ToString());
            }
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            string port = "COM" + textBox1.Text;
            bool isOpened = false;
            serP = new System.IO.Ports.SerialPort(port, 9600, System.IO.Ports.Parity.None, 8, System.IO.Ports.StopBits.One);
            try
            {
                serP.Open();
                isOpened = true;
            }
            catch (Exception ex)
            {
                isOpened = false;
                textBox3.Text = (ex.ToString());
            }

            if (isOpened)
            {
                timer.Tick += new EventHandler(timer_Tick);
                timer.Interval = 3;
                timer.Start();
                textBox3.Text = "Podłączenie do portu " + port + " przebiegło pomyślnie";
            }
        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            serP.Close();
            timer.Stop();
            textBox3.Text = "Połączenie na porcie COM zostało zakończone";
        }



    }
}
