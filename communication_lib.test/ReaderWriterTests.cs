using MessagePack;
using NUnit.Framework;
using System.Collections.Generic;

namespace communication_lib.test
{
    public class ReaderWriterTests
    {
        [Test]
        public void String([Values("", "1", "nagyonhosszú")]string value)
        {
            var bytes = MessagePackSerializer.Serialize(value);
            var actual = MessagePackSerializer.Deserialize<string>(bytes);

            Assert.AreEqual(value, actual);
        }

        [Test]
        public void Unicode([Values("🦊", "😺🙈☠")]string value)
        {
            var bytes = MessagePackSerializer.Serialize(value);
            var actual = MessagePackSerializer.Deserialize<string>(bytes);

            Assert.AreEqual(value, actual);
        }
        [Test]
        public void Bytes([Values((uint)0,(uint)42, (uint)666666)] uint value)
        {
            var bytes = MessagePackSerializer.Serialize(value);
            var actual = MessagePackSerializer.Deserialize<uint>(bytes);

            Assert.AreEqual(value, actual);
        }

        public static IEnumerable<List<string>> stringLists = new List<List<string>>
        {
            new List<string> {},
            new List<string> { "egy" },
            new List<string> { "", "kettő", "🦊" }
        };

        [Test]
        public void GetStringList([ValueSource("stringLists")]List<string> value)
        {

            var bytes = MessagePackSerializer.Serialize(value);
            var actual = MessagePackSerializer.Deserialize<List<string>>(bytes);

            Assert.AreEqual(value, actual);

        }

        public static IEnumerable<List<FoodContains>> structList = new List<List<FoodContains>>
        {
            new List<FoodContains> {},
            new List<FoodContains> { new FoodContains { FoodId = 1, FoodName = "Gulyas", FoodPrice = 2000, Amount = 3} },
            new List<FoodContains> { new FoodContains { },
                new FoodContains { FoodId = 10, FoodName = "🦊", FoodPrice = 7000, Amount = 1},
                new FoodContains{ FoodId = 22, FoodName ="😺", FoodPrice = 10000, Amount=10  } }
        };

        [Test]
        public void GetStructList([ValueSource("structList")]List<FoodContains> value)
        {
            var bytes = MessagePackSerializer.Serialize(value);
            var actual = MessagePackSerializer.Deserialize<List<FoodContains>>(bytes);

            Assert.AreEqual(value, actual);
        }
    }
}